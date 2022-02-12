#include <iostream>
#include <filesystem>
#include <windows.h>
#include <ShlObj.h>
#include <wil/resource.h>

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
}
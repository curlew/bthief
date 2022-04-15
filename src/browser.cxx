#include "browser.hxx"

#include <ShlObj.h>
#include <wil/resource.h>
#include <windows.h>

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

std::string format_time(sqlite3_int64 t) {
    // time since epoch is stored by chrome as microseconds but by FILETIME as 100-nanosecond intervals
    // https://docs.microsoft.com/en-us/windows/win32/sysinfo/file-times
    // https://source.chromium.org/chromium/chromium/src/+/main:base/time/time.h;l=505;drc=721e6d70189ce1350f8ff733a02c98b9bc8e8251
    t *= 10;

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
}
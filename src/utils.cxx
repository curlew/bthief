#include "utils.hxx"

#include <windows.h>
#include <wil/resource.h>

std::filesystem::path find_folder(REFKNOWNFOLDERID id) {
    wil::unique_cotaskmem_string path;
    SHGetKnownFolderPath(id, KF_FLAG_DONT_UNEXPAND, NULL, &path);
    return std::filesystem::path(path.get());
}

std::string narrow(const wchar_t *s) {
    if (wcslen(s) == 0) {
        return std::string();
    }

    int chars = WideCharToMultiByte(CP_UTF8, 0, s, (int)wcslen(s), NULL, 0, NULL, NULL);
    std::string ret(chars, 0);
    WideCharToMultiByte(CP_UTF8, 0, s, (int)wcslen(s), &ret[0], chars, NULL, NULL);
    return ret;
}
std::string narrow(const std::wstring &s) {
    if (s.empty()) {
        return std::string();
    }

    int chars = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0, NULL, NULL);
    std::string ret(chars, 0);
    WideCharToMultiByte(CP_UTF8, 0, s.c_str(), (int)s.size(), &ret[0], chars, NULL, NULL);
    return ret;
}

std::wstring widen(const char *s) {
    if (strlen(s) == 0) {
        return std::wstring();
    }

    int chars = MultiByteToWideChar(CP_UTF8, 0, s, (int)strlen(s), NULL, 0);
    std::wstring ret(chars, 0);
    MultiByteToWideChar(CP_UTF8, 0, s, (int)strlen(s), &ret[0], chars);
    return ret;
}
std::wstring widen(const std::string &s) {
    if (s.empty()) {
        return std::wstring();
    }

    int chars = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), NULL, 0);
    std::wstring ret(chars, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &ret[0], chars);
    return ret;
}

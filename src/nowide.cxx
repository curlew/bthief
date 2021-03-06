#include "nowide.hxx"

#include <windows.h>

std::string narrow(const wchar_t *s) {
    if (wcslen(s) == 0) { return std::string(); }

    int chars = WideCharToMultiByte(CP_UTF8, 0, s, wcslen(s), NULL, 0, NULL, NULL);
    std::string ret(chars, 0);
    WideCharToMultiByte(CP_UTF8, 0, s, wcslen(s), &ret[0], chars, NULL, NULL);
    return ret;
}
std::string narrow(const std::wstring &s) {
    if (s.empty()) { return std::string(); }

    int chars = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), s.size(), NULL, 0, NULL, NULL);
    std::string ret(chars, 0);
    WideCharToMultiByte(CP_UTF8, 0, s.c_str(), s.size(), &ret[0], chars, NULL, NULL);
    return ret;
}

std::wstring widen(const char *s) {
    if (strlen(s) == 0) { return std::wstring(); }

    int chars = MultiByteToWideChar(CP_UTF8, 0, s, strlen(s), NULL, 0);
    std::wstring ret(chars, 0);
    MultiByteToWideChar(CP_UTF8, 0, s, strlen(s), &ret[0], chars);
    return ret;
}
std::wstring widen(const std::string &s) {
    if (s.empty()) { return std::wstring(); }

    int chars = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.size(), NULL, 0);
    std::wstring ret(chars, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), s.size(), &ret[0], chars);
    return ret;
}
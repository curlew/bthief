#ifndef BTHIEF_UTILS_HXX
#define BTHIEF_UTILS_HXX

#include <filesystem>
#include <string>
#include <ShlObj.h>

// wrapper around SHGetKnownFolderPath
std::filesystem::path find_folder(REFKNOWNFOLDERID);

// helper functions for working with the UTF-16 Windows API
std::string  narrow(const wchar_t *s);
std::string  narrow(const std::wstring &s);
std::wstring widen(const char *s);
std::wstring widen(const std::string &s);

#endif // ifndef BTHIEF_UTILS_HXX

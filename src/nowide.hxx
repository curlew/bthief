/**
 * @file nowide.hxx
 * @brief Helper functions for working with the UTF-16 Windows API.
 */

#ifndef NOWIDE_HXX_INCLUDED
#define NOWIDE_HXX_INCLUDED

#include <string>

std::string  narrow(const wchar_t *s);
std::string  narrow(const std::wstring &s);
std::wstring widen(const char *s);
std::wstring widen(const std::string &s);

#endif // ifndef NOWIDE_HXX_INCLUDED

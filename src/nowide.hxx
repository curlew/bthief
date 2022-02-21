#pragma once

#include <string>

std::string  narrow(const wchar_t *s);
std::string  narrow(const std::wstring &s);
std::wstring widen(const char *s);
std::wstring widen(const std::string &s);
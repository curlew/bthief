#ifndef LIBRARY_HXX_INCLUDED
#define LIBRARY_HXX_INCLUDED

#include <filesystem>
#include <format>
#include <functional>
#include <string>
#include <windows.h>
#include <wil/resource.h>

/**
 * Represents a shared library.
 */
class library {
    std::filesystem::path m_path;
    wil::unique_hmodule m_lib;

public:
    library(const std::filesystem::path &);

    template<typename T>
    std::function<T> function(const std::string &name) {
        FARPROC f = GetProcAddress(m_lib.get(), name.c_str());
        if (f == NULL) {
            throw std::runtime_error(std::format("Error loading function '{}' from library '{}'", name, m_path.string()));
        }
        return std::function<T>(reinterpret_cast<T *>(f));
    }
};

#endif // ifndef LIBRARY_HXX_INCLUDED

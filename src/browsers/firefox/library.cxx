#include "library.hxx"

#include "utils.hxx"

library::library(const std::filesystem::path &path)
    : m_path{path} {
    // search for dependencies in the DLL's parent directory as well as in the default dirs
    HMODULE lib = LoadLibraryExW(widen(path.string()).c_str(), NULL,
                                 LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

    m_lib = wil::unique_hmodule(lib);
}

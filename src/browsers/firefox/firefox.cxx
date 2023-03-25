#include "firefox.hxx"

#include "nowide.hxx"
#include <iostream>
#include <vector>
#include <ShlObj.h>
#include <windows.h>
#include <wil/resource.h>

std::expected<std::vector<login>, browser_error> firefox::get_logins(void) {
    std::cout << "NSS path: " << find_nss().value_or("not found") << "\n";

    return {};
}

std::optional<std::filesystem::path> firefox::find_nss(void) {
    wil::unique_cotaskmem_string program_files_path;
    SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DONT_UNEXPAND, NULL,
                         &program_files_path);

    namespace fs = std::filesystem;

    fs::path base_path = program_files_path.get();

    // TODO:
    std::vector<fs::path> search_dirs = {
        "Mozilla Firefox",
        "Firefox Nightly",
    };

    for (auto &search_dir : search_dirs) {
        fs::path p = base_path / search_dir / fs::path("nss3.dll");
        if (fs::exists(p)) {
            return p;
        }
    }

    return std::nullopt;
}

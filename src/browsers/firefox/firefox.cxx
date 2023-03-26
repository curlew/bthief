#include "firefox.hxx"

#include "library.hxx"
#include "nowide.hxx"
#include <iostream>
#include <vector>
#include <ShlObj.h>
#include <windows.h>
#include <wil/resource.h>

class nss_library : private library {
public:
    nss_library(const std::filesystem::path &path) : library(path) {}

    std::function<const char *(void)> NSS_GetVersion = function<const char *(void)>("NSS_GetVersion");
};

std::expected<std::vector<login>, browser_error> firefox::get_logins(void) {
    std::filesystem::path nss_path;
    if (auto maybe_nss_path = find_nss()) {
        nss_path = *maybe_nss_path;
    } else {
        std::cerr << "couldn't find nss\n";
        return std::unexpected(browser_error::file_not_found);
    }
    std::cout << "NSS path: " << nss_path.string() << "\n";

    std::vector<std::filesystem::path> profiles;
    if (auto maybe_profiles = find_profiles()) {
        profiles = *maybe_profiles;
    } else {
        std::cerr << "no profiles found\n";
        return std::unexpected(browser_error::file_not_found);
    }

    for (const auto &profile : profiles) {
        std::cout << " - found profile: " << profile.string() << "\n";
    }

    std::optional<nss_library> maybe_nss;
    try {
        maybe_nss = std::optional<nss_library>(nss_path);
    } catch (std::runtime_error &e) {
        std::cerr << "NSS exception: " << e.what() << "\n";
        return std::unexpected(browser_error::file_not_found);
    }
    nss_library &nss = *maybe_nss;
    std::cout << "NSS version: " << nss.NSS_GetVersion() << "\n";

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

std::optional<std::vector<std::filesystem::path>> firefox::find_profiles(void) {
    wil::unique_cotaskmem_string roaming_appdata_path;
    SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DONT_UNEXPAND, NULL,
                         &roaming_appdata_path);
    std::filesystem::path base_path = roaming_appdata_path.get();
    base_path = base_path / "Mozilla" / "Firefox" / "Profiles";

    std::vector<std::filesystem::path> profiles;
    for (const auto &profile : std::filesystem::directory_iterator(base_path)) {
        profiles.emplace_back(profile.path());
    }

    return !profiles.empty() ? std::make_optional(profiles) : std::nullopt;
}

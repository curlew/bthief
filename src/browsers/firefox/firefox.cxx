#include "firefox.hxx"

#include "crypt.hxx"
#include "library.hxx"
#include "nowide.hxx"
#include "nss.hxx"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include <ShlObj.h>
#include <windows.h>
#include <wil/resource.h>
#include <nlohmann/json.hpp>

namespace {
class nss_library : private library {
public:
    nss_library(const std::filesystem::path &path)
        : library(path) { }

    // symbols for runtime dynamic linking
    // clang-format off
    std::function<::NSS_Initialize>    NSS_Initialize    = function<::NSS_Initialize>("NSS_Initialize");
    std::function<::NSS_Shutdown>      NSS_Shutdown      = function<::NSS_Shutdown>("NSS_Shutdown");
    std::function<::SECITEM_AllocItem> SECITEM_AllocItem = function<::SECITEM_AllocItem>("SECITEM_AllocItem");
    std::function<::SECITEM_ZfreeItem> SECITEM_ZfreeItem = function<::SECITEM_ZfreeItem>("SECITEM_ZfreeItem");
    std::function<::PK11SDR_Decrypt>   PK11SDR_Decrypt   = function<::PK11SDR_Decrypt>("PK11SDR_Decrypt");
    // clang-format on
};

class nss {
    nss_library &m_nss_lib;

public:
    nss(nss_library &nss_lib, const std::filesystem::path &profile)
        : m_nss_lib{nss_lib} {
        if (m_nss_lib.NSS_Initialize(profile.string().c_str(), "", "", SECMOD_DB, NSS_INIT_READONLY) != SECSuccess) {
            throw std::runtime_error("nss: NSS_Initialize() failed"); // TODO:
        }
    }

    ~nss() {
        m_nss_lib.NSS_Shutdown();
    }

    // non-copyable due to nss_library reference
    nss(const nss &) = delete;
    nss &operator=(const nss &) = delete;

    std::string decrypt(const std::string &ciphertext_b64) {
        auto SECItem_deleter = [&](SECItem *i) { m_nss_lib.SECITEM_ZfreeItem(i, true); };
        using unique_SECItem = std::unique_ptr<SECItem, decltype(SECItem_deleter)>;

        std::vector<uint8_t> ciphertext = b64_decode(ciphertext_b64);

        SECItem in = {};
        in.data = ciphertext.data();
        in.len = (unsigned int)ciphertext.size();
        unique_SECItem out(m_nss_lib.SECITEM_AllocItem(NULL, NULL, 0), SECItem_deleter);
        // std::copy(ciphertext.begin(), ciphertext.end(), in->data);

        if (m_nss_lib.PK11SDR_Decrypt(&in, out.get(), NULL) != SECSuccess) {
            return ""; // TODO:
        }
        return std::string((const char *)out->data, out->len);
    }
};
} // namespace

std::expected<std::vector<login>, browser_error> firefox::get_logins(void) {
    std::filesystem::path nss_path;
    if (auto maybe_nss_path = find_nss()) {
        nss_path = *maybe_nss_path;
    } else {
        return std::unexpected(browser_error::file_not_found);
    }

    std::optional<nss_library> maybe_nss_lib;
    try {
        maybe_nss_lib = std::optional<nss_library>(nss_path);
    } catch (std::runtime_error &e) {
        std::cerr << "NSS exception: " << e.what() << "\n";
        return std::unexpected(browser_error::file_not_found); // TODO:
    }
    nss_library &nss_lib = *maybe_nss_lib;

    std::vector<std::filesystem::path> profiles;
    if (auto maybe_profiles = find_profiles()) {
        profiles = *maybe_profiles;
    } else {
        return std::unexpected(browser_error::file_not_found);
    }

    std::vector<login> logins;
    for (const auto &profile : profiles) {
        std::ifstream logins_file(profile / "logins.json");
        if (!logins_file) {
            continue;
        }

        nss db(nss_lib, profile);

        using json = nlohmann::json;
        json j = json::parse(logins_file);
        for (const auto &login_entry : j.at("logins")) {
            // if (login_entry.at("hostname") == "chrome://FirefoxAccounts") {} // TODO:

            using ms = std::chrono::milliseconds;
            using time_point = std::chrono::system_clock::time_point;

            login l;
            l.url = login_entry.at("hostname");
            l.username = db.decrypt(login_entry.at("encryptedUsername"));
            l.password = db.decrypt(login_entry.at("encryptedPassword"));
            l.date_created = time_point(ms(login_entry.at("timeCreated").get<uint64_t>()));
            l.date_last_used = time_point(ms(login_entry.at("timeLastUsed").get<uint64_t>()));
            l.date_password_modified = time_point(ms(login_entry.at("timePasswordChanged").get<uint64_t>()));
            logins.emplace_back(l);
        }
    }

    return logins;
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

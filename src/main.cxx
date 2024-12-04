#include "browsers/chrome/chrome.hxx"
#include "browsers/firefox/firefox.hxx"
#include "utils.hxx"
#include <array>
#include <iostream>
#include <windows.h>
#include <nlohmann/json.hpp>

namespace {
bool option_exists(int argc, char *argv[], const std::string &option) {
    return std::find(argv, argv + argc, option) != argv + argc;
}
} // namespace

int main(int argc, char *argv[]) {
    SetConsoleOutputCP(CP_UTF8);

    const auto appdata_local = find_folder(FOLDERID_LocalAppData);
    const auto appdata_roaming = find_folder(FOLDERID_RoamingAppData);

    const auto browsers = std::to_array<std::pair<std::string, std::unique_ptr<browser>>>({
        {"Chrome",        chrome::construct("chrome.exe", appdata_local / "Google" / "Chrome" / "User Data")},
        {"Chrome Beta",   chrome::construct("chrome.exe", appdata_local / "Google" / "Chrome Beta" / "User Data")},
        {"Chrome Canary", chrome::construct("chrome.exe", appdata_local / "Google" / "Chrome SxS" / "User Data")},
        {"Chromium",      chrome::construct("chrome.exe", appdata_local / "Chromium" / "User Data")},
        {"Edge",          chrome::construct("msedge.exe", appdata_local / "Microsoft" / "Edge" / "User Data")},
        {"Brave",         chrome::construct("brave.exe",  appdata_local / "BraveSoftware" / "Brave-Browser" / "User Data")},
        {"Opera",         chrome::construct("opera.exe",  appdata_roaming / "Opera Software" / "Opera Stable")},
        {"Firefox",       std::make_unique<firefox>()}, // TODO: static constructor
    });

    const bool json_mode = option_exists(argc, argv, "-j");
    nlohmann::ordered_json logins_json; // only used in JSON mode

    for (const auto &[name, browser] : browsers) {
        if (!browser) { continue; } // browser not found

        auto logins = browser->get_logins();
        if (!logins) {
            // error getting logins
            continue; // TODO:
        }

        if (json_mode) {
            logins_json[name] = logins.value();
        } else {
            std::cout << name << ":\n";

            for (auto &login : logins.value()) {
                std::cout << login << "\n";
            }

            std::cout << "\n";
        }
    }

    if (json_mode) {
        std::cout << logins_json.dump(4) << std::endl;
    }
}

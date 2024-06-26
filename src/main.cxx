#include "browsers/chrome/chrome.hxx"
#include "browsers/firefox/firefox.hxx"
#include "utils.hxx"
#include <array>
#include <chrono>
#include <format>
#include <iostream>
#include <utility>
#include <windows.h>

int main() {
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

    for (auto &[name, browser] : browsers) {
        std::cout << "[" << name << "] " << (browser ? "FOUND:\n" : "NOT FOUND\n");
        if (!browser) { continue; } // browser not found

        auto logins = browser->get_logins();
        if (!logins.has_value()) {
            std::cerr << "Error getting logins\n";
            continue;
        }

        const auto format_time_point = [](std::chrono::system_clock::time_point &t) {
            return std::format("{:%F %T %Z}", std::chrono::round<std::chrono::seconds>(t));
        };

        for (auto &l : logins.value()) {
            std::string date_last_used_str = l.date_last_used.time_since_epoch().count() == 0
                                                 ? "never"
                                                 : format_time_point(l.date_last_used);

            std::cout << "  - " << l.url << "\n"
                      << "    - Username: [" << l.username << "]\n"
                      << "    - Password: [" << l.password << "]\n"
                      << "    - Created: " << format_time_point(l.date_created) << "\n"
                      << "    - Last used: " << date_last_used_str << "\n"
                      << "    - Password last modified: " << format_time_point(l.date_password_modified) << "\n";
        }
    }
}

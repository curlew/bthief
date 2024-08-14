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

        for (auto &l : logins.value()) {
            std::cout << l << "\n";
        }
    }
}

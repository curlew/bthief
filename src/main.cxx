#include "browsers/browser.hxx"
#include "browsers/chrome/chrome.hxx"
#include "browsers/firefox/firefox.hxx"
#include <chrono>
#include <format>
#include <iostream>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::unique_ptr<browser> browsers[] = {
        std::make_unique<chrome>(),
        std::make_unique<firefox>(),
    };

    for (auto &b : browsers) {
        auto logins = b->get_logins();
        if (!logins.has_value()) {
            std::cerr << "[" << b->get_name() << "] ERROR: couldn't get logins\n";
            continue;
        }

        const auto format_time_point = [](std::chrono::system_clock::time_point &tp) {
            return std::format("{:%F %T %Z}", std::chrono::round<std::chrono::seconds>(tp));
        };

        std::cout << "##############################  " << b->get_name() << "  ##############################\n\n";

        for (auto &l : logins.value()) {
            std::string date_last_used_str = l.date_last_used.time_since_epoch().count() == 0
                                                 ? "never"
                                                 : format_time_point(l.date_last_used);

            std::cout << "  - " << l.url << "\n"
                      << "    - Username: [" << l.username << "]\n"
                      << "    - Password: [" << l.password << "]\n"
                      << "    - Created: " << format_time_point(l.date_created) << "\n"
                      << "    - Last used: " << date_last_used_str << "\n"
                      << "    - Password last modified: " << format_time_point(l.date_password_modified) << "\n"
                      << "\n";
        }
    }
}

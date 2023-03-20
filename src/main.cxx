#include "browser.hxx"
#include "chrome.hxx"
#include <format>
#include <iostream>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::unique_ptr<browser> browsers[] = {
        std::make_unique<chrome>(),
    };

    for (auto &b : browsers) {
        auto logins = b->get_logins();
        if (!logins.has_value()) {
            std::cerr << "[" << typeid(*b).name() << "] ERROR: couldn't get logins\n";
            continue;
        }

        for (auto &l : logins.value()) {
            uint64_t last_used_timestamp = l.date_last_used.time_since_epoch().count();
            std::string last_used = last_used_timestamp == 0
                                        ? "never"
                                        : std::format("{:%F %T%z}", l.date_last_used);

            std::cout << l.url << "\n"
                      << "  " << l.username << ":" << l.password << "\n"
                      << "  created: " << std::format("{:%F %T%z}", l.date_created) << "\n"
                      << "  last used: " << last_used << "\n"
                      << "  password last modified: " << std::format("{:%F %T%z}", l.date_password_modified) << "\n\n";
        }
    }
}

#include "chrome.hxx"
#include "firefox.hxx"
#include "login.hxx"
#include <ctime>
#include <iostream>
#include <memory>
#include <vector>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::unique_ptr<browser> browsers[] = {
        std::make_unique<chrome>(),
        //std::make_unique<firefox>(),
    };

    for (auto &b : browsers) {
        if (!b->is_valid()) { continue; }

        std::vector<login> logins = b->get();
        for (auto &l : logins) {
            const std::time_t date_created = std::chrono::system_clock::to_time_t(l.date_created);
            std::cout << l.url << "\n"
                      << "    " << l.username << ":" << l.password << "\n"
                      << "    created " << std::put_time(std::gmtime(&date_created), "%F %T UTC") << "\n"
                      << "\n";
        }
    }
}

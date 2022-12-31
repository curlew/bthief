#include "chrome.hxx"
#include "firefox.hxx"
#include <memory>
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::unique_ptr<browser> browsers[] = {
        std::make_unique<chrome>(),
        std::make_unique<firefox>(),
    };

    for (auto &b : browsers) {
        b->get();
    }
}

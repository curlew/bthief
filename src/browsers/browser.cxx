#include "browser.hxx"

std::ostream &operator<<(std::ostream &os, const login &l) {
    const auto format_time_point = [](const std::chrono::system_clock::time_point &t) {
        return std::format("{:%F %T %Z}", std::chrono::round<std::chrono::seconds>(t));
    };

    std::string date_last_used_str = l.date_last_used.time_since_epoch().count() == 0
                                            ? "never"
                                            : format_time_point(l.date_last_used);

    return os << "  - " << l.url << "\n"
              << "    - Username: [" << l.username << "]\n"
              << "    - Password: [" << l.password << "]\n"
              << "    - Times used: " << l.times_used << "\n"
              << "    - Created: " << format_time_point(l.date_created) << "\n"
              << "    - Last used: " << date_last_used_str << "\n"
              << "    - Password last modified: " << format_time_point(l.date_password_modified);
}

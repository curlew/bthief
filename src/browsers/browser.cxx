#include "browser.hxx"

namespace {

std::string format_time_point(const std::chrono::system_clock::time_point &t) {
    if (t.time_since_epoch().count() == 0) {
        return "never";
    }

    return std::format("{:%F %T %Z}", std::chrono::floor<std::chrono::seconds>(t));
};

} // namespace

std::ostream &operator<<(std::ostream &os, const login &l) {
    return os << "  - " << l.url << "\n"
              << "    - Username: [" << l.username << "]\n"
              << "    - Password: [" << l.password << "]\n"
              << "    - Times used: " << l.times_used << "\n"
              << "    - Created: " << format_time_point(l.date_created) << "\n"
              << "    - Last used: " << format_time_point(l.date_last_used) << "\n"
              << "    - Password last modified: " << format_time_point(l.date_password_modified);
}

void to_json(nlohmann::ordered_json &j, const login &l) {
    using seconds = std::chrono::seconds;

    j = nlohmann::ordered_json{
        {"url",                    l.url},
        {"username",               l.username},
        {"password",               l.password},
        {"times_used",             l.times_used},
        {"date_created",           std::chrono::floor<seconds>(l.date_created.time_since_epoch()).count()},
        {"date_last_used",         std::chrono::floor<seconds>(l.date_last_used.time_since_epoch()).count()},
        {"date_password_modified", std::chrono::floor<seconds>(l.date_password_modified.time_since_epoch()).count()},
    };
}

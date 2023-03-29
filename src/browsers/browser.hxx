#ifndef BTHIEF_BROWSERS_BROWSER_HXX
#define BTHIEF_BROWSERS_BROWSER_HXX

#include <chrono>
#include <expected>
#include <string>
#include <vector>

struct login {
    std::string url;
    std::string username;
    std::string password;
    std::chrono::system_clock::time_point date_created;
    std::chrono::system_clock::time_point date_last_used;
    std::chrono::system_clock::time_point date_password_modified;
};

/**
 * Error conditions of browser::get_logins().
 */
enum class browser_error {
    bcrypt_error,
    file_not_found,
    json_parse_error,
    sqlite_error,
};

/**
 * Abstract base class representing a browser.
 * Derived classes implement the get_logins() member function.
 */
class browser {
public:
    virtual ~browser() = default;
    virtual std::string get_name(void) const = 0;
    virtual std::expected<std::vector<login>, browser_error> get_logins(void) = 0;
};

#endif // ifndef BTHIEF_BROWSERS_BROWSER_HXX

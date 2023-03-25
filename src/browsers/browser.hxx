#ifndef BROWSER_HXX_INCLUDED
#define BROWSER_HXX_INCLUDED

#include "login.hxx"
#include <expected>
#include <vector>

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
    virtual std::expected<std::vector<login>, browser_error> get_logins(void) = 0;
};

#endif // ifndef BROWSER_HXX_INCLUDED

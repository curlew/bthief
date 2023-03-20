#ifndef LOGIN_HXX_INCLUDED
#define LOGIN_HXX_INCLUDED

#include <chrono>
#include <string>

struct login {
    std::string url;
    std::string username;
    std::string password;
    std::chrono::system_clock::time_point date_created;
    std::chrono::system_clock::time_point date_last_used;
    std::chrono::system_clock::time_point date_password_modified;
};

#endif // ifndef LOGIN_HXX_INCLUDED

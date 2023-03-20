#ifndef LOGIN_HXX_INCLUDED
#define LOGIN_HXX_INCLUDED

#include <chrono>
#include <string>

struct login {
    std::string url;
    std::string username, password;
    std::chrono::system_clock::time_point date_created, date_last_used;
};

#endif // ifndef LOGIN_HXX_INCLUDED

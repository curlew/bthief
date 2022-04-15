#pragma once

#include <filesystem>
#include <sqlite3.h>

namespace chrome {
extern std::filesystem::path key_path;

extern std::filesystem::path logins_path;
extern std::filesystem::path cookies_path;

std::string format_time(sqlite3_int64);
}
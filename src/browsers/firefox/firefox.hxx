#ifndef FIREFOX_HXX_INCLUDED
#define FIREFOX_HXX_INCLUDED

#include "browsers/browser.hxx"

#include <expected>
#include <filesystem>
#include <optional>
#include <vector>

class firefox : public browser {
public:
    std::expected<std::vector<login>, browser_error> get_logins(void) override;

private:
    std::optional<std::filesystem::path> find_nss(void);
    std::optional<std::vector<std::filesystem::path>> find_profiles(void);
};

#endif // ifndef FIREFOX_HXX_INCLUDED

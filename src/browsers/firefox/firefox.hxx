#ifndef BTHIEF_BROWSERS_FIREFOX_FIREFOX_HXX
#define BTHIEF_BROWSERS_FIREFOX_FIREFOX_HXX

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

#endif // ifndef BTHIEF_BROWSERS_FIREFOX_FIREFOX_HXX

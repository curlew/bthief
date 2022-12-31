#ifndef CHROME_HXX_INCLUDED
#define CHROME_HXX_INCLUDED

#include "browser.hxx"

class chrome : public browser {
public:
    void get(void) override;
};

#endif // ifndef CHROME_HXX_INCLUDED

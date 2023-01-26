#ifndef BROWSER_HXX_INCLUDED
#define BROWSER_HXX_INCLUDED

#include "login.hxx"
#include <vector>

class browser {
public:
    virtual ~browser() = default;
    virtual std::vector<login> get(void) = 0;
    bool is_valid(void) const;

protected:
    bool m_valid = false;
};

#endif // ifndef BROWSER_HXX_INCLUDED

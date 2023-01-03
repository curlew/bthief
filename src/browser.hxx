#ifndef BROWSER_HXX_INCLUDED
#define BROWSER_HXX_INCLUDED

class browser {
public:
    virtual ~browser() = default;
    virtual void get(void) = 0;
    bool is_valid(void) const;

protected:
    bool m_valid = false;
};

#endif // ifndef BROWSER_HXX_INCLUDED

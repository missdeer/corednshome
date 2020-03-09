#ifndef ScopedGuard_H
#define ScopedGuard_H

#include <functional>

class ScopedGuard
{
    std::function<void(void)> m_f;

public:
    ScopedGuard()                    = delete;
    ScopedGuard(ScopedGuard &&)      = delete;
    ScopedGuard(const ScopedGuard &) = delete;
    ScopedGuard &operator=(ScopedGuard &&) = delete;
    ScopedGuard &operator=(const ScopedGuard &) = delete;
    explicit ScopedGuard(std::function<void(void)> f);
    ~ScopedGuard();
};

#endif // ScopedGuard_H

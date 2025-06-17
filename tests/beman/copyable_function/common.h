#ifndef COMMON_H
#define COMMON_H

inline bool flag = false;
inline void func() { flag = true; }

struct Callable {
    std::initializer_list<int> il;
    Callable() {}
    Callable(std::initializer_list<int> il) : il(il) {}

    int operator()() { return 42; }

    int operator()() const noexcept { return 43; }
};

#endif

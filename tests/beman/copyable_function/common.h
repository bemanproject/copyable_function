#ifndef COMMON_H
#define COMMON_H

inline bool flag = false;
inline void func() { flag = true; }

struct Callable {
    std::initializer_list<int> il;
    Callable() {}
    Callable(std::initializer_list<int> il) : il(il) {}
    ~Callable() = default;

    int operator()() noexcept { return 42; }
    int operator()() const noexcept { return 43; }
};

struct LargeCallable {
    long arr[4];
    LargeCallable()  = default;
    ~LargeCallable() = default;

    int operator()() noexcept { return 1; }
    int operator()() const noexcept { return 2; }
};

#endif

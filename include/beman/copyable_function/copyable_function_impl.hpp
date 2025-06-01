#include <utility>
#include <functional>
#include <type_traits>
#include <memory>

#ifndef _CONST
#define _CONST
#endif

#ifndef _REF
#define _REF
#define INVOKE_QUALS _CONST&

#else
#define INVOKE_QUALS _CONST _REF

#endif

#ifndef _COPYABLE_FUNC_NOEXCEPT
#define _COPYABLE_FUNC_NOEXCEPT false
#endif

namespace beman {

template <class _Fp>
class base;

template <class R, class... Args>
class base<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)> {
  public:
    virtual ~base()                                                                      = default;
    virtual R     operator()(Args... args) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT) = 0;
    virtual base* clone()                                                                = 0;
    virtual void  clone(base*)                                                           = 0;
    virtual void  destroy()                                                              = 0;
};

template <class Fp, class Fd>
class fn_ptr;
template <class Fp, class R, class... Args>
class fn_ptr<Fp, R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)>
    : public base<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)> {

  private: 
    Fp func;
    using Fun = fn_ptr<Fp, R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)>;
    using Alloc = std::allocator<Fun>;
    Alloc a;
  public:

    fn_ptr(Fp&& f) : func(std::move(f)) {}
    fn_ptr(const Fp& f) : func(f) {}

    template <class... A>
    fn_ptr(A&&... args) : func(args...) {}

    ~fn_ptr() = default;

    R operator()(Args... args) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT) { return func(args...); }

    base<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)>* clone() {
        std::unique_ptr<Fun> ptr(a.allocate(1));
        ::new ((void*)ptr.get()) Fun(func);
        return ptr.release();
    }

    void clone(base<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)>* ptr) {
        ::new ((void*)ptr) fn_ptr(func);
    }

    void destroy() { func.~Fp(); }
};

template <class Fp>
class function_holder;
template <class R, class... Args>
class function_holder<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)> {
    typedef base<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)> Fn;
    Fn*                                                                    fn   = nullptr;
    static constexpr int                                                   size = 3 * sizeof(void*);
    std::byte                                                              buffer[size];

  public:
    function_holder() = default;
    function_holder(std::nullptr_t) : fn(nullptr) {}

    ~function_holder() {
        if (fn != nullptr) {
            fn->destroy();
        }
    }

    function_holder(const function_holder& other) {
        if (other.fn == nullptr) {
            fn = nullptr;
        } else {
            fn = other.fn->clone();
        }
    }

    function_holder(function_holder&& other) {
        if (other.fn == nullptr) {
            fn = nullptr;
        } else {
            fn       = other.fn;
            other.fn = nullptr;
        }
    }

    template <class F, std::enable_if_t<!std::is_same<std::decay_t<F>, function_holder>::value, int> = 0>
    function_holder(F&& f) {
        using DecayFn = std::decay_t<F>;
        typedef fn_ptr<DecayFn, R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)> Fun;
        if (sizeof(F) <= sizeof(buffer)) {
            fn = ::new ((void*)buffer) Fun(std::move(f));
        } else {
            fn = new Fun(std::move(f));
        }
    }

    template <class Func, class... _Args>
    function_holder(std::in_place_type_t<Func>, _Args&&... args) {
        typedef fn_ptr<Func, R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)> Fun;
        fn = new Fun(std::forward<_Args>(args)...);
    }

    template <class Func, class U, class... _Args>
    function_holder(std::in_place_type_t<Func>, std::initializer_list<U> il, _Args&&... args) {
        typedef fn_ptr<Func, R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)> Fun;
        fn = new Fun(il, std::forward<_Args>(args)...);
    }

    function_holder& operator=(function_holder&& other) {
        if (other.fn == nullptr) {
            fn = nullptr;
        } else {
            fn = other.fn->clone();
        }
        return *this;
    }

    void reset() { fn = nullptr; }

    R operator()(Args&&... args) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT) {
        return std::invoke_r<R>(static_cast<Fn INVOKE_QUALS>(*fn), std::forward<Args>(args)...);
    }

    bool operator==(std::nullptr_t) { return fn == nullptr; }

    bool operator!=(std::nullptr_t) { return fn != nullptr; }

    void swap(function_holder& other) {
        if (other.fn == fn) {
            return;
        } else {
            std::swap(fn, other.fn);
        }
    }
};

template <class... T>
class copyable_function;
template <class R, class... Args>
class copyable_function<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)> {
  private:
    function_holder<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)> fn;
    template <class T>
    static constexpr bool is_callable_from =
        _COPYABLE_FUNC_NOEXCEPT
            ? (std::is_nothrow_invocable_r_v<R, T _CONST _REF, Args...> &&
               std::is_nothrow_invocable_r_v<R, T INVOKE_QUALS, Args...>)
            : (std::is_invocable_r_v<R, T _CONST _REF, Args...> && std::is_invocable_r_v<R, T INVOKE_QUALS, Args...>);

  public:
    using result_type            = R;
    copyable_function() noexcept = default;
    copyable_function(std::nullptr_t) noexcept : fn(nullptr) {}

    copyable_function(const copyable_function& other) : fn(other.fn) {}

    copyable_function(copyable_function&& other) noexcept : fn(std::move(other.fn)) {}

    template <class Func>
    copyable_function(Func f) : fn(std::move(f)) {}

    template <class Func, class... _Args>
    explicit copyable_function(std::in_place_type_t<Func>, _Args&&... args)
        : fn(std::in_place_type_t<Func>{}, std::forward<_Args>(args)...) {}

    template <class Func, class U, class... _Args>
    explicit copyable_function(std::in_place_type_t<Func>, std::initializer_list<U> il, _Args&&... args)
        : fn(std::in_place_type_t<Func>{}, il, std::forward<_Args>(args)...) {}

    copyable_function& operator=(const copyable_function& other) {
        fn = other.fn;
        return *this;
    }

    copyable_function& operator=(copyable_function&& other) {
        fn = std::move(other.fn);
        return *this;
    }

    copyable_function& operator=(std::nullptr_t) noexcept {
        fn.reset();
        return *this;
    }

    template <class F>
    copyable_function& operator=(F&& f) {
        fn = f;
        return *this;
    }

    R operator()(Args&&... args) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT) {
        using Type = function_holder<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)>;
        return std::invoke_r<R>(static_cast<Type INVOKE_QUALS>(fn), std::forward<Args>(args)...);
    }

    void swap(copyable_function& other) noexcept { fn.swap(other.fn); }

    friend void swap(copyable_function& f1, copyable_function& f2) noexcept { f1.swap(f2); }

    friend bool operator==(const copyable_function& func, std::nullptr_t) noexcept { return true; }
};
} // namespace beman

#undef _CONST
#undef _REF
#undef _COPYABLE_FUNC_NOEXCEPT
#undef INVOKE_QUALS
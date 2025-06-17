#include <utility>
#include <functional>
#include <type_traits>
#include <memory>
#include <new>

#include "helper.h"

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

#ifndef _USE_CUSTOM_VTABLE
#define _USE_CUSTOM_VTABLE true
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
    using Fun   = fn_ptr<Fp, R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)>;
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
        using alloc_traits = std::allocator_traits<Alloc>;
        using alloc_rebind = alloc_traits::template rebind_alloc<fn_ptr>;
        alloc_rebind         ap(a);
        std::unique_ptr<Fun> ptr(ap.allocate(1));
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
            fn->~Fn();
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
        if (sizeof(DecayFn) <= sizeof(buffer)) {
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

    static constexpr std::size_t BufferSize = 3 * sizeof(void*);
    static constexpr std::size_t Alignment  = sizeof(void*);
    using BufferType                        = Buffer<BufferSize, Alignment>;
    using VTableType                        = VTable<BufferType, R, Args...>;

    mutable BufferType __buffer;

    template <class Functor>
    static constexpr VTableType __vtable = {
        .call = [](BufferType& __buffer, Args&&... args) -> R {
            return std::invoke_r<R>(static_cast<Functor INVOKE_QUALS>(*__buffer.get_ptr<Functor>()),
                                    std::forward<Args>(args)...);
        },
        .destroy =
            [](BufferType& __buffer) {
                using DecayT = std::decay_t<Functor>;
                std::destroy_at(__buffer.get_ptr<Functor>());
                if (sizeof(DecayT) > BufferSize) {
                    ::operator delete(*reinterpret_cast<void**>(__buffer.get_ptr<DecayT>()),
                                      std::align_val_t{alignof(DecayT)});
                }
            },
        .clone =
            [](BufferType& from, BufferType& to) {
                using DecayT = std::decay_t<Functor>;
                auto* ptr    = new DecayT(*from.get_ptr<DecayT>());
                to.template construct<DecayT>(*from.get_ptr<DecayT>());
            }};

    template <class _Func, class... _Args>
    void construct(_Args&&... args) {
        using DecayType = std::decay_t<_Func>;
        __vtable_ptr    = &__vtable<DecayType>;
        __buffer.template construct<DecayType>(std::forward<_Args>(args)...);
    }

    const VTableType* __vtable_ptr = nullptr;

  public:
    using result_type            = R;
    copyable_function() noexcept = default;
    copyable_function(std::nullptr_t) noexcept
#if _USE_CUSTOM_VTABLE == false
        : fn(nullptr) {
    }
#else
    {
        __vtable_ptr = nullptr;
    }
#endif

    ~copyable_function() {
#if _USE_CUSTOM_VTABLE == true
        if (__vtable_ptr) {
            __vtable_ptr->destroy(__buffer);
            __vtable_ptr = nullptr;
        }
#endif
    }

    copyable_function(const copyable_function& other)
#if _USE_CUSTOM_VTABLE == false
        : fn(other.fn)
#endif
    {
#if _USE_CUSTOM_VTABLE == true
        __vtable_ptr = other.__vtable_ptr;
        if (__vtable_ptr) {
            __vtable_ptr->clone(other.__buffer, __buffer);
        }
#endif
    }

    copyable_function(copyable_function&& other) noexcept
#if _USE_CUSTOM_VTABLE == false
        : fn(std::move(other.fn))
#else
        : __vtable_ptr(std::move(other.__vtable_ptr)),
          __buffer(std::move(other.__buffer))
#endif
    {
        other.__vtable_ptr = nullptr;
    }

    template <class Func>
    copyable_function(Func&& f)
        requires(!std::is_same_v<std::remove_cvref_t<Func>, copyable_function>)
#if _USE_CUSTOM_VTABLE == false
        : fn(std::forward<Func>(f))
#endif
    {
#if _USE_CUSTOM_VTABLE == true
        using DecayType = std::decay_t<Func>;
        __vtable_ptr    = &__vtable<DecayType>;
        __buffer.template construct<DecayType>(std::forward<Func>(f));
#endif
    }

    template <class Func, class... _Args>
    explicit copyable_function(std::in_place_type_t<Func>, _Args&&... args)
#if _USE_CUSTOM_VTABLE == false
        : fn(std::in_place_type_t<Func>{}, std::forward<_Args>(args)...)
#endif
    {
#if _USE_CUSTOM_VTABLE == true
        construct<Func>(std::forward<_Args>(args)...);
#endif
    }

    template <class Func, class U, class... _Args>
    explicit copyable_function(std::in_place_type_t<Func>, std::initializer_list<U> il, _Args&&... args)
#if _USE_CUSTOM_VTABLE == false
        : fn(std::in_place_type_t<Func>{}, il, std::forward<_Args>(args)...)
#endif
    {
#if _USE_CUSTOM_VTABLE == true
        construct<Func>(il, std::forward<_Args>(args)...);
#endif
    }

    copyable_function& operator=(const copyable_function& other) {
#if _USE_CUSTOM_VTABLE == false
        fn = other.fn;
        return *this;
#else
        copyable_function(other).swap(*this);
        return *this;
#endif
    }

    copyable_function& operator=(copyable_function&& other) {
#if _USE_CUSTOM_VTABLE == false
        fn = std::move(other.fn);
        return *this;
#else
        copyable_function(std::move(other)).swap(*this);
        return *this;
#endif
    }

    copyable_function& operator=(std::nullptr_t) noexcept {
#if _USE_CUSTOM_VTABLE == false
        fn.reset();
        return *this;
#else
        __vtable_ptr->destroy(__buffer);
        __vtable_ptr = nullptr;
        return *this;
#endif
    }

    template <class F>
    copyable_function& operator=(F&& f) {
#if _USE_CUSTOM_VTABLE == false
        fn = f;
        return *this;
#else
        copyable_function(std::forward<F>(f)).swap(*this);
        return *this;
#endif
    }

    R operator()(Args&&... args) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT) {
#if _USE_CUSTOM_VTABLE == false
        using Type = function_holder<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)>;
        return std::invoke_r<R>(static_cast<Type INVOKE_QUALS>(fn), std::forward<Args>(args)...);
#else
        const auto __call = static_cast<R (*)(BufferType&, Args...)>(__vtable_ptr->call);
        return __call(__buffer, std::forward<Args>(args)...);
#endif
    }

    void swap(copyable_function& other) noexcept {
#if _USE_CUSTOM_VTABLE == false
        fn.swap(other.fn);
#else
        std::swap(__vtable_ptr, other.__vtable_ptr);
        std::swap(__buffer, other.__buffer);
#endif
    }

    friend void swap(copyable_function& f1, copyable_function& f2) noexcept { f1.swap(f2); }

    friend bool operator==(const copyable_function& func, std::nullptr_t) noexcept { return true; }
};
} // namespace beman

#undef _CONST
#undef _REF
#undef _COPYABLE_FUNC_NOEXCEPT
#undef INVOKE_QUALS
#undef _USE_CUSTOM_VTABLE

#include <utility>
#include <functional>
#include <type_traits>
#include <memory>
#include <new>

#include "copyable_function_helper.h"

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

template <class... T>
class copyable_function;
template <class R, class... Args>
class copyable_function<R(Args...) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT)> {
  private:
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
                ::operator delete(reinterpret_cast<void**>(__buffer.get_ptr<DecayT>()),
                                  std::align_val_t{alignof(DecayT)});
            },
        .clone =
            [](BufferType& from, BufferType& to) {
                using DecayT = std::decay_t<Functor>;
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
    copyable_function(std::nullptr_t) noexcept { __vtable_ptr = nullptr; }

    ~copyable_function() {
        if (__vtable_ptr) {
            __vtable_ptr->destroy(__buffer);
            __vtable_ptr = nullptr;
        }
    }

    copyable_function(const copyable_function& other) {
        __vtable_ptr = other.__vtable_ptr;
        if (__vtable_ptr) {
            __vtable_ptr->clone(other.__buffer, __buffer);
        }
    }

    copyable_function(copyable_function&& other) noexcept
        : __vtable_ptr(std::move(other.__vtable_ptr)), __buffer(std::move(other.__buffer)) {
        other.__vtable_ptr = nullptr;
    }

    template <class Func>
    copyable_function(Func&& f)
        requires(!std::is_same_v<std::remove_cvref_t<Func>, copyable_function> && is_callable_from<std::decay_t<Func>>)
    {
        using DecayType = std::decay_t<Func>;
        __vtable_ptr    = &__vtable<DecayType>;
        __buffer.template construct<DecayType>(std::forward<Func>(f));
    }

    template <class T, class... _Args>
    explicit copyable_function(std::in_place_type_t<T>, _Args&&... args)
        requires(is_callable_from<std::decay_t<T>>)
    {
        construct<T>(std::forward<_Args>(args)...);
    }

    template <class T, class U, class... _Args>
    explicit copyable_function(std::in_place_type_t<T>, std::initializer_list<U> il, _Args&&... args)
        requires(is_callable_from<std::decay_t<T>>)
    {
        construct<T>(il, std::forward<_Args>(args)...);
    }

    copyable_function& operator=(const copyable_function& other) {
        copyable_function(other).swap(*this);
        return *this;
    }

    copyable_function& operator=(copyable_function&& other) {
        copyable_function(std::move(other)).swap(*this);
        return *this;
    }

    copyable_function& operator=(std::nullptr_t) noexcept {
        __vtable_ptr->destroy(__buffer);
        __vtable_ptr = nullptr;
        return *this;
    }

    template <class F>
    copyable_function& operator=(F&& f) {
        copyable_function(std::forward<F>(f)).swap(*this);
        return *this;
    }

    R operator()(Args&&... args) _CONST _REF noexcept(_COPYABLE_FUNC_NOEXCEPT) {
        const auto __call = static_cast<R (*)(BufferType&, Args...)>(__vtable_ptr->call);
        return __call(__buffer, std::forward<Args>(args)...);
    }

    void swap(copyable_function& other) noexcept {
        std::swap(__vtable_ptr, other.__vtable_ptr);
        std::swap(__buffer, other.__buffer);
    }

    friend void swap(copyable_function& f1, copyable_function& f2) noexcept { f1.swap(f2); }

    friend bool operator==(const copyable_function& func, std::nullptr_t) noexcept { return true; }
};
} // namespace beman

#undef _CONST
#undef _REF
#undef _COPYABLE_FUNC_NOEXCEPT
#undef INVOKE_QUALS

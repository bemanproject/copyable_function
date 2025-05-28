// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_COPYABLE_FUNCTION_HPP
#define BEMAN_COPYABLE_FUNCTION_HPP

// There are macros: CONST, NOEXCEPT, REF

// no const, no noexcept, no ref
#include "copyable_function_impl.hpp"

// no const, no noexcept, lvalue ref
#define _REF &
#include "copyable_function_impl.hpp"

// no const, no noexcept, rvalue ref
#define _REF &&
#include "copyable_function_impl.hpp"

// no const, noexcept, no ref
#define _COPYABLE_FUNC_NOEXCEPT true
#include "copyable_function_impl.hpp"

// no const, noexcept, lvalue ref
#define _REF &
#define _COPYABLE_FUNC_NOEXCEPT true
#include "copyable_function_impl.hpp"

// no const, noexcept, rvalue ref
#define _REF &&
#define _COPYABLE_FUNC_NOEXCEPT true
#include "copyable_function_impl.hpp"

// const, no noexcept, no ref
#define _CONST const
#include "copyable_function_impl.hpp"

// const, no noexcept, lvalue ref
#define _CONST const
#define _REF &
#include "copyable_function_impl.hpp"

// const, no noexcept, rvalue ref
#define _CONST const
#define _REF &&
#include "copyable_function_impl.hpp"


// const, noexcept, no ref
#define _CONST const
#define _COPYABLE_FUNC_NOEXCEPT true
#include "copyable_function_impl.hpp"

// const, noexcept, lvalue ref
#define _CONST const
#define _REF &
#define _COPYABLE_FUNC_NOEXCEPT true
#include "copyable_function_impl.hpp"

// const, noexcept, rvalue ref
#define _CONST const
#define _REF &&
#define _COPYABLE_FUNC_NOEXCEPT true
#include "copyable_function_impl.hpp"


#endif // BEMAN_COPYABLE_FUNCTION_IDENTITY_HPP


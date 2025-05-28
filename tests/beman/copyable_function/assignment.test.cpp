// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/copyable_function/copyable_function.hpp>
#include "common.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <iostream> 

TEST(CallTest, CopyAssignment) {
    beman::copyable_function<int()> f(Callable{});
    beman::copyable_function<int()> f2 = f;
    int x = f2();
    EXPECT_EQ(x, 42);
}

TEST(CallTest, MoveAssignment) {
    beman::copyable_function<int()> f(Callable{});
    beman::copyable_function<int()> f2 = std::move(f);
    int x = f2();
    EXPECT_EQ(x, 42);
}

TEST(CallTest, NullPtrAssignment) {
    beman::copyable_function<int()> f(Callable{});
    int x = f();
    EXPECT_EQ(x, 42);
    f = nullptr; 
    EXPECT_EQ(f == nullptr, true);
}

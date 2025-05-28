// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/copyable_function/copyable_function.hpp>
#include "common.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <iostream> 

int fn1()
{
    return 42; 
}

int fn2()
{
    return 43;
}

TEST(CallTest, SwapTest) {
    beman::copyable_function<int()> f1(fn1);
    beman::copyable_function<int()> f2(fn2);
    f1.swap(f2);
    int x = f1(); 
    int y = f2(); 
    EXPECT_EQ(x, 43);
    EXPECT_EQ(y, 42);
}

TEST(CallTest, SwapFriendFunctionTest) {
    beman::copyable_function<int()> f1(fn1);
    beman::copyable_function<int()> f2(fn2);
    swap(f1, f2);
    int x = f1(); 
    int y = f2(); 
    EXPECT_EQ(x, 43);
    EXPECT_EQ(y, 42);
}

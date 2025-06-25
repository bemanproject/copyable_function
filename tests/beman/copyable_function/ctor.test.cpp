// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/copyable_function/copyable_function.hpp>
#include "common.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>

int fn() { return 42; }

TEST(CtorTest, CtorTestWithLambda) {
    auto                            lambda = []() -> int { return 42; };
    beman::copyable_function<int()> f(lambda);
    int                             x = f();
    EXPECT_EQ(x, 42);
}

TEST(CtorTest, CtorTestWithFunctionPointer) {
    beman::copyable_function<int()> f(fn);
    int                             x = f();
    EXPECT_EQ(x, 42);
}

TEST(CtorTest, CtorTestWithCallable) {
    beman::copyable_function<int()> f(Callable{});
    int                             x = f();
    EXPECT_EQ(x, 42);
}

TEST(CtorTest, CtorTestCopyConstructor) {
    beman::copyable_function<int()> f(Callable{});
    beman::copyable_function<int()> f2(f);
    int                             x = f2();
    EXPECT_EQ(x, 42);
}

TEST(CtorTest, CtorTestMoveConstructor) {
    beman::copyable_function<int()> f2(beman::copyable_function<int()>(Callable{}));
    int                             x = f2();
    EXPECT_EQ(x, 42);
}

TEST(CtorTest, CtorTestInplaceType) {
    beman::copyable_function<int()> f2(std::in_place_type_t<Callable>{});
    int                             x = f2();
    EXPECT_EQ(x, 42);
}

TEST(CtorTest, CtorTestInplaceTypeHavingInitializerList) {
    beman::copyable_function<int()> f2(std::in_place_type_t<Callable>{}, {1, 2, 3});
    int                             x = f2();
    EXPECT_EQ(x, 42);
}

TEST(CtorTest, CtorTestWithLargeObject) {
    beman::copyable_function<int()> f2(LargeCallable{});
    int                             x = f2();
    EXPECT_EQ(x, 1);
}

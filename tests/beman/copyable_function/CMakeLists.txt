# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

include(GoogleTest)

add_executable(beman.copyable_function.tests.copyable_function)
target_sources(
    beman.copyable_function.tests.copyable_function
    PRIVATE assignment.test.cpp ctor.test.cpp call.test.cpp swap.test.cpp
)

target_link_libraries(
    beman.copyable_function.tests.copyable_function
    PRIVATE beman::copyable_function GTest::gtest GTest::gtest_main
)

gtest_add_tests(beman.copyable_function.tests.copyable_function "" AUTO)

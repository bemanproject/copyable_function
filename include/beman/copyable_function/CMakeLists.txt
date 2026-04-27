# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

target_sources(
    beman.copyable_function
    PUBLIC
        FILE_SET HEADERS
            FILES
                copyable_function.hpp
                copyable_function_helper.h
                copyable_function_impl.hpp
)

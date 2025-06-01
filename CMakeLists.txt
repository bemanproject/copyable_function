# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.25)

project(
    beman.copyable_function # CMake Project Name, which is also the name of the top-level
    # targets (e.g., library, executable, etc.).
    DESCRIPTION "A Beman library copyable_function"
    LANGUAGES CXX
    VERSION 0.1.0
)
set(CMAKE_CXX_STANDARD 26)

enable_testing()

# [CMAKE.SKIP_TESTS]
option(
    BEMAN_COPYABLE_FUNCTION_BUILD_TESTS
    "Enable building tests and test infrastructure. Default: ON. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)


option(
    BEMAN_COPYABLE_FUNCTION_INSTALL_CONFIG_FILE_PACKAGE
    "Enable creating and installing a CMake config-file package. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

include(FetchContent)
include(GNUInstallDirs)

if(BEMAN_COPYABLE_FUNCTION_BUILD_TESTS)
    # Fetch GoogleTest
    FetchContent_Declare(
        GTest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG
            6910c9d9165801d8827d628cb72eb7ea9dd538c5 # release-1.16.0
        EXCLUDE_FROM_ALL
    )
    set(INSTALL_GTEST OFF) # Disable GoogleTest installation
    FetchContent_MakeAvailable(GTest)
endif()

add_subdirectory(src/beman/copyable_function)

if(BEMAN_COPYABLE_FUNCTION_BUILD_TESTS)
    add_subdirectory(tests/beman/copyable_function)
endif()

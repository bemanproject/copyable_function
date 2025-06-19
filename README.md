# beman.copyable_function: A Beman Library Implementation of copyable_function (P2548)

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable-next-line line-length -->
![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg) ![Continuous Integration Tests](https://github.com/bemanproject/exemplar/actions/workflows/ci_tests.yml/badge.svg) ![Lint Check (pre-commit)](https://github.com/bemanproject/exemplar/actions/workflows/pre-commit.yml/badge.svg)

`beman.copyable_function` is a minimal C++ library conforming to [The Beman Standard](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_STANDARD.md).
This can be used as a template for those intending to write Beman libraries.
It may also find use as a minimal and modern  C++ project structure.

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_LIBRARY_MATURITY_MODEL.md#under-development-and-not-yet-ready-for-production-use)

## Usage

`std::copyable_function` is a type-erased function wrapper
that can represent any copyable callable matching
the function signature R(Args...)

### Usage: default projection in constrained algorithms

The following code snippet illustrates how we can achieve a default projection using `beman::exemplar::identity`:

```cpp
#include <beman/copyable_function/copyable_function.hpp>

namespace exe = beman::exemplar;

// a Callable object
struct Callable {
    int operator()() { return 42; }
    int operator()() const noexcept { return 43; }
};

int main()
{
    beman::copyable_function<int()> f(Callable{});
    int x = f();
    return 0;
}

```

Full runnable examples can be found in [`examples/`](examples/).

## Dependencies

### Build Environment

This project requires at least the following to build:

* C++17
* CMake 3.25

This project pulls [Google Test](https://github.com/google/googletest)
from GitHub as a development dependency for its testing framework,
thus requiring an active internet connection to configure.
You can disable this behavior by setting cmake option
[`BEMAN_EXEMPLAR_BUILD_TESTS`](#beman_exemplar_build_tests) to `OFF`
when configuring the project.

However,
some examples and tests will not be compiled
unless provided compiler support **C++20** or ranges capabilities enabled.

> [!TIP]
>
> In the logs you will be able to see if there are any examples that aren't enabled
> due to compiler capabilities or the configured C++ version.
>
> Below is an example:
>
> ```txt
> -- Looking for __cpp_lib_ranges
> -- Looking for __cpp_lib_ranges - not found
> CMake Warning at examples/CMakeLists.txt:12 (message):
>   Missing range support! Skip: identity_as_default_projection
>
>
> Examples to be built: identity_direct_usage
> ```

### Supported Platforms

This project officially supports:

* GNU GCC Compiler \[version 12-14\]
* LLVM Clang++ Compiler \[version 17-20\]
* AppleClang compiler on Mac OS
* MSVC compiler on Windows

> [!NOTE]
>
> Versions outside of this range would likely work as well,
> especially if you're using a version above the given range
> (e.g. HEAD/ nightly).
> These development environments are verified using our CI configuration.

## Development

### Develop using GitHub Codespace

This project supports [GitHub Codespace](https://github.com/features/codespaces)
via [Development Containers](https://containers.dev/),
which allows rapid development and instant hacking in your browser.
We recommend you using GitHub codespace to explore this project as this
requires minimal setup.

You can create a codespace for this project by clicking this badge:

[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/bemanproject/exemplar)

For more detailed documentation regarding creating and developing inside of
GitHub codespaces, please reference [this doc](https://docs.github.com/en/codespaces/).

> [!NOTE]
>
> The codespace container may take up to 5 minutes to build and spin-up,
> this is normal as we need to build a custom docker container to setup
> an environment appropriate for beman projects.

### Develop locally on your machines

<details>
<summary> For Linux based systems </summary>

Beman libraries requires [recent versions of CMake](#build-environment),
we advice you download CMake directly from [CMake's website](https://cmake.org/download/)
or install via the [Kitware apt library](https://apt.kitware.com/).

A [supported compiler](#supported-platforms) should be available from your package manager.
Alternatively you could use an install script from official compiler vendors.

Here is an example of how to install the latest stable version of clang
as per [the official LLVM install guide](https://apt.llvm.org/).

```bash
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
```

</details>

<details>
<summary> For MacOS based systems </summary>

Beman libraries require [recent versions of CMake](#build-environment).
You can use [`Homebrew`](https://brew.sh/) to install the latest major version of CMake.

```bash
brew install cmake
```

A [supported compiler](#supported-platforms) is also available from brew.

For example, you can install latest major release of Clang++ compiler as:

```bash
brew install llvm
```

</details>

### Configure and Build the Project Using CMake Presets

This project recommends using [CMake Presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)
to configure, build and test the project.
Appropriate presets for major compilers have been included by default.
You can use `cmake --list-presets` to see all available presets.

Here is an example to invoke the `gcc-debug` preset.

```shell
cmake --workflow --preset gcc-debug
```

Generally, there's two kinds of presets, `debug` and `release`.

The `debug` presets are designed to aid development, so it has debugging
instrumentation enabled and as many sanitizers turned on as possible.

> [!NOTE]
>
> The set of sanitizer supports are different across compilers.
> You can checkout the exact set compiler arguments by looking at the toolchain
> files under the [`cmake`](cmake/) directory.

The `release` presets are designed for use in production environments,
thus it has the highest optimization turned on (e.g. `O3`).

### Configure and Build Manually

While [CMake Presets](#configure-and-build-the-project-using-cmake-presets) are
convenient, you might want to set different configuration or compiler arguments
than any provided preset supports.

To configure, build and test the project with extra arguments,
you can run this sets of command.

```bash
cmake -B build -S . -DCMAKE_CXX_STANDARD=20 # Your extra arguments here.
cmake --build build
ctest --test-dir build
```

> [!IMPORTANT]
>
> Beman projects are
> [passive projects](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_STANDARD.md#cmake),
> therefore,
> you will need to specify C++ version via `CMAKE_CXX_STANDARD`
> when manually configuring the project.

### Project specific configure arguments

When configuring the project manually,
you can pass an array of project specific CMake configs to customize your build.

Project specific options are prefixed with `BEMAN_EXEMPLAR`.
You can see the list of available options with:

```bash
cmake -LH | grep "BEMAN_EXEMPLAR" -C 2
```

<details>

<summary> Details of CMake arguments. </summary>

#### `BEMAN_EXEMPLAR_BUILD_TESTS`

Enable building tests and test infrastructure. Default: ON.
Values: { ON, OFF }.

You can configure the project to have this option turned off via:

```bash
cmake -B build -S . -DCMAKE_CXX_STANDARD=20 -DBEMAN_EXEMPLAR_BUILD_TESTS=OFF
```

> [!TIP]
> Because this project requires Google Tests as part of its development
> dependency,
> disable building tests avoids the project from pulling Google Tests from
> GitHub.

#### `BEMAN_EXEMPLAR_BUILD_EXAMPLES`

Enable building examples. Default: ON. Values: { ON, OFF }.

</details>

## Integrate beman.copyable_function into your project

To use `beman.copyable_function` in your C++ project,
include an appropriate `beman.copyable_function` header from your source code.

```c++
#include <beman/copyable_function/copyable_function.hpp>
```

> [!NOTE]
>
> `beman.copyable_function` headers are to be included with the `beman/copyable_function/` directories prefixed.
> It is not supported to alter include search paths to spell the include target another way. For instance,
> `#include <copyable_function.hpp>` is not a supported interface.

How you will link your project against `beman.copyable_function` will depend on your build system.
CMake instructions are provided in following sections.

### Linking your project to beman.exemplar with CMake

For CMake based projects,
you will need to use the `beman.copyable_function` CMake module
to define the `beman::copyable_function` CMake target:

```cmake
find_package(beman.copyable_function REQUIRED)
```

You will also need to add `beman::copyable_function` to the link libraries of
any libraries or executables that include beman.copyable_function's header file.

```cmake
target_link_libraries(yourlib PUBLIC beman::copyable_function)
```

### Produce beman.copyable_function static library locally

You can include copyable_function's headers locally
by producing a static `libbeman.copyable_function.a` library.

```bash
cmake --workflow --preset gcc-release
cmake --install build/gcc-release --prefix /opt/beman.copyable_function
```

This will generate such directory structure at `/opt/beman.copyable_function`.

```txt
/opt/beman.copyable_function
├── include
│   └── beman
│       └── copyable_function
│           └── copyable_function.hpp
└── lib
    └── libbeman.copyable_function.a
```

## Contributing

Please do!
You encourage you to checkout our [contributor's guide](docs/README.md).
Issues and pull requests are appreciated.

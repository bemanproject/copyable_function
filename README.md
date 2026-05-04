# beman.copyable_function: A Beman Library Implementation of copyable_function

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable-next-line line-length -->
![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg) ![Continuous Integration Tests](https://github.com/bemanproject/copyable_function/actions/workflows/ci_tests.yml/badge.svg) ![Lint Check (pre-commit)](https://github.com/bemanproject/copyable_function/actions/workflows/pre-commit-check.yml/badge.svg) [![Coverage](https://coveralls.io/repos/github/bemanproject/copyable_function/badge.svg?branch=main)](https://coveralls.io/github/bemanproject/copyable_function?branch=main) ![Standard Target](https://github.com/bemanproject/beman/blob/main/images/badges/cpp26.svg)

`beman.copyable_function` is a type-erased function wrapper that can represent any copyable callable matching
the function signature R(Args...). The library conforms to the [The Beman Standard](https://github.com/bemanproject/beman/blob/main/docs/beman_standard.md).

**Implements**: [`copyable_function` (P2548)](https://wg21.link/P2548R6)

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md#under-development-and-not-yet-ready-for-production-use)

## License

`beman.copyable_function` is licensed under the Apache License v2.0 with LLVM Exceptions.

## Usage

The following code snippet illustrates `copyable_function`:

```cpp
#include <beman/copyable_function/copyable_function.hpp>

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

* A C++ compiler that conforms to the C++17 standard or greater
* CMake 3.30 or later
* (Test Only) GoogleTest

You can disable building tests by setting CMake option `BEMAN_COPYABLE_FUNCTION_BUILD_TESTS` to
`OFF` when configuring the project.

### Supported Platforms

| Compiler   | Version | C++ Standards | Standard Library  |
|------------|---------|---------------|-------------------|
| GCC        | 16-13   | C++26, C++23  | libstdc++         |
| GCC        | 12      | C++23         | libstdc++         |
| Clang      | 22-19   | C++26, C++23  | libstdc++, libc++ |
| Clang      | 18      | C++26, C++23  | libc++            |
| Clang      | 18      | C++23         | libstdc++         |
| Clang      | 17      | C++26, C++23  | libc++            |
| AppleClang | latest  | C++26, C++23  | libc++            |
| MSVC       | latest  | C++23         | MSVC STL          |

## Development

See the [Contributing Guidelines](CONTRIBUTING.md).

## Integrate beman.copyable_function into your project

### Build

You can build copyable_function using a CMake workflow preset:

```bash
cmake --workflow --preset gcc-release
```

To list available workflow presets, you can invoke:

```bash
cmake --list-presets=workflow
```

For details on building beman.copyable_function without using a CMake preset, refer to the
[Contributing Guidelines](CONTRIBUTING.md).

### Installation

#### Vcpkg

The preferred way to install copyable_function is via vcpkg. To do so, after installing vcpkg
itself, you need to add support for the Beman project's [vcpkg
registry](https://github.com/bemanproject/vcpkg-registry) by configuring a
`vcpkg-configuration.json` file (which copyable_function [provides](vcpkg-configuration.json)).

Then, simply run `vcpkg install beman-copyable-function`.

#### Manual

To install beman.copyable_function globally after building with the `gcc-release` preset, you can
run:

```bash
sudo cmake --install build/gcc-release
```

Alternatively, to install to a prefix, for example `/opt/beman`, you can run:

```bash
sudo cmake --install build/gcc-release --prefix /opt/beman
```

This will generate the following directory structure:

```txt
/opt/beman
├── include
│   └── beman
│       └── copyable_function
│           ├── copyable_function.hpp
│           └── ...
└── lib
    └── cmake
        └── beman.copyable_function
            ├── beman.copyable_function-config-version.cmake
            ├── beman.copyable_function-config.cmake
            └── beman.copyable_function-targets.cmake
```

### CMake Configuration

If you installed beman.copyable_function to a prefix, you can specify that prefix to your CMake
project using `CMAKE_PREFIX_PATH`; for example, `-DCMAKE_PREFIX_PATH=/opt/beman`.

You need to bring in the `beman.copyable_function` package to define the `beman::copyable_function` CMake
target:

```cmake
find_package(beman.copyable_function REQUIRED)
```

You will then need to add `beman::copyable_function` to the link libraries of any libraries or
executables that include `beman.copyable_function` headers.

```cmake
target_link_libraries(yourlib PUBLIC beman::copyable_function)
```

### Using beman.copyable_function

To use `beman.copyable_function` in your C++ project,
include an appropriate `beman.copyable_function` header from your source code.

```c++
#include <beman/copyable_function/copyable_function.hpp>
```

> [!NOTE]
>
> `beman.copyable_function` headers are to be included with the `beman/copyable_function/` prefix.
> Altering include search paths to spell the include target another way (e.g.
> `#include <copyable_function.hpp>`) is unsupported.

## Contributing

Please do!
You encourage you to checkout our [contributor's guide](docs/README.md).
Issues and pull requests are appreciated.

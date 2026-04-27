# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(ALL_EXAMPLES sample_usage)
message("Examples to be built: ${ALL_EXAMPLES}")

foreach(example ${ALL_EXAMPLES})
    add_executable(beman.copyable_function.examples.${example})
    target_sources(
        beman.copyable_function.examples.${example}
        PRIVATE ${example}.cpp
    )
    target_link_libraries(
        beman.copyable_function.examples.${example}
        PRIVATE beman::copyable_function
    )
endforeach()

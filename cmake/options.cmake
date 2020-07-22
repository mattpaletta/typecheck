# This file contains all of the command-line set_option_if_not_sets for CMake builds.
# This is kept separately so it's easier to view at a glance, and by using code, the 'documentation' is always up-to-date.

include(cmake/utils.cmake)

set_option_if_not_set(ENABLE_ASAN "Enable Address Sanitizer" OFF)
set_option_if_not_set(ENABLE_UBSAN "Enable Undefined Behaviour Sanitizer" OFF)
set_option_if_not_set(ENABLE_IWYU "Use include-what-you-use" ON)
set_option_if_not_set(ENABLE_LWYU "Use link-what-you-use" ON)
set_option_if_not_set(ENABLE_CLANG_TIDY "Use clang-tidy" ON)
set_option_if_not_set(TYPECHECK_FIX_ERRORS "Only relevant if ENABLE_CLANG_TIDY enabled, sets --fix flag on clang-tidy" ON)

set_option_if_not_set(ENABLE_CPP_CHECK "Use cppcheck" ON)
set_option_if_not_set(TYPECHECK_WERROR "Use Werror" OFF)
set_option_if_not_set(TYPECHECK_BUILD_TESTS "Build tests, defaults to true if in-source directory" OFF)

set_option_if_not_set(TYPECHECK_PRINT_DEBUG_CONSTRAINTS "Prints debug information when creating constraints" OFF)
set_option_if_not_set(TYPECHECK_PRINT_SHORT_DEBUG "Prints shorter debug description information, only active if TYPECHECK_PRINT_DEBUG_CONSTRAINTS=ON" ON)

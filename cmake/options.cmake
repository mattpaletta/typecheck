# This file contains all of the command-line set_option_if_not_sets for CMake builds.
# This is kept separately so it's easier to view at a glance, and by using code, the 'documentation' is always up-to-date.

include(cmake/utils.cmake)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	message("In-Source Build")
	set(default_if_in_dir ON)
else()
	message("Subproject Source Build")
	set(default_if_in_dir OFF)
endif()

set(in_source_msg "Defaults to `ON` if in-source build")

set_option_if_not_set(ENABLE_ASAN "Enable Address Sanitizer" OFF)
set_option_if_not_set(ENABLE_UBSAN "Enable Undefined Behaviour Sanitizer" OFF)
set_option_if_not_set(ENABLE_IWYU "Use include-what-you-use - ${in_source_msg}" ${default_if_in_dir})
set_option_if_not_set(ENABLE_LWYU "Use link-what-you-use - ${in_source_msg}" ${default_if_in_dir})
set_option_if_not_set(ENABLE_CLANG_TIDY "Use clang-tidy - ${in_source_msg}" ${default_if_in_dir})
set_option_if_not_set(TYPECHECK_FIX_ERRORS "Only relevant if ENABLE_CLANG_TIDY enabled, sets --fix flag on clang-tidy - ${in_source_msg}" ${default_if_in_dir})

set_option_if_not_set(ENABLE_CPP_CHECK "Use cppcheck - ${in_source_msg}" ${default_if_in_dir})
set_option_if_not_set(TYPECHECK_WERROR "Use Werror" OFF)
set_option_if_not_set(TYPECHECK_BUILD_TESTS "Build tests - ${in_source_msg}" ${default_if_in_dir})
set_option_if_not_set(TYPECHECK_ENABLE_COVERAGE "Build code coverage targets ${in_source_msg}" OFF)

set_option_if_not_set(TYPECHECK_PRINT_DEBUG_CONSTRAINTS "Prints debug information when creating constraints" OFF)
set_option_if_not_set(TYPECHECK_PRINT_SHORT_DEBUG "Prints shorter debug description information, only active if TYPECHECK_PRINT_DEBUG_CONSTRAINTS=ON" ON)

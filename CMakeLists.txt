cmake_minimum_required (VERSION 3.16)
project(typecheck LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Loads all of the options
include(cmake/options.cmake)
if (TYPECHECK_ENABLE_COVERAGE)
	# Only require lcov if code coverage enabled
	include(cmake/CodeCoverage.cmake)
	set(TYPECHECK_LCOV_ARGS -q --rc lcov_branch_coverage=1 --no-external)
	set(COVERAGE_EXCLUDES "_deps/*" "typecheck_protos/*" "${CMAKE_CURRENT_BINARY_DIR}/_deps/*" ${CMAKE_CURRENT_BINARY_DIR}/typecheck_protos/*)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type." FORCE)

	append_coverage_compiler_flags()
endif()

if (ENABLE_ASAN)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if (ENABLE_UBSAN)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON )
if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json)
endif()

set(TYPECHECK_EXTRA_FLAGS "")
include(cmake/utils.cmake) # Defines: add_supported_cxx_flags
if (MSVC)
    add_supported_cxx_flags("/Wall")
    add_supported_cxx_flags("/W3")
else()
	if (CMAKE_BUILD_TYPE STREQUAL DEBUG)
		add_supported_cxx_flags("-fPIC")
	endif()

    add_supported_cxx_flags("-Wall")
    add_supported_cxx_flags("-Wextra")
    add_supported_cxx_flags("-Wconversion")
    add_supported_cxx_flags("-Wunreachable-code")
    add_supported_cxx_flags("-Wuninitialized")
    add_supported_cxx_flags("-pedantic")
    add_supported_cxx_flags("-pedantic-errors")
    add_supported_cxx_flags("-Wno-error=unused-variable")
    add_supported_cxx_flags("-Wshadow")
    add_supported_cxx_flags("-Wfloat-equal")
    add_supported_cxx_flags("-Wduplicated-cond")
    add_supported_cxx_flags("-Wno-error=duplicated-branches")
    add_supported_cxx_flags("-Wlogical-op")
    add_supported_cxx_flags("-Wrestrict")
    add_supported_cxx_flags("-Wnull-dereference")
    add_supported_cxx_flags("-Wuseless-cast")
    add_supported_cxx_flags("-Wno-error=double-promotion")
    add_supported_cxx_flags("-Wformat=2")
    add_supported_cxx_flags("-Wformat-truncation")
    add_supported_cxx_flags("-Wformat-overflow")
    add_supported_cxx_flags("-Wshift-overflow")
    add_supported_cxx_flags("-Wundef")
    add_supported_cxx_flags("-fno-common")
    add_supported_cxx_flags("-Wswitch-enum")
    add_supported_cxx_flags("-Wno-error=effc++")
    add_supported_cxx_flags("-fanalyzer")
endif()

if (TYPECHECK_WERROR)
	message("Enabling WError")
	if (MSVC)
		add_supported_cxx_flags("/WX")
	else()
		set(TYPECHECK_EXTRA_FLAGS ${TYPECHECK_EXTRA_FLAGS} -Werror)
	endif()
endif()

include(cmake/fetch_extern.cmake)
set(CATCH_VERSION 0f12995501ee01d3d2bdd9f4978bb28b5f670bab)
set(BLOATY_VERSION 3cf5c3feca15ab88730d6526b2f06302597926b5)

# This includes protobuf
if (${TYPECHECK_ENABLE_BLOATY})
	fetch_extern(bloaty https://github.com/google/bloaty.git ${BLOATY_VERSION})
endif()

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.hpp)
file(GLOB_RECURSE INC_FILES include/*.hpp)

if (ENABLE_IWYU)
	find_program(iwyu_path NAMES include-what-you-use iwyu REQUIRED)
	message("Include-What-You-Use Enabled: ${iwyu_path}")
	set(iwhy_path_and_options "${iwyu_path} --mapping_file=${CMAKE_CURRENT_BINARY_DIR}/typecheck.imp")
endif()

if (ENABLE_LWYU)
	message("Link-What-You-Use Enabled")
	set(CMAKE_LINK_WHAT_YOU_USE CACHE BOOL ON)
endif()

if (ENABLE_CPP_CHECK)
    set(CMAKE_CXX_CLANG_TIDY "")

	find_program(CMAKE_CXX_CPPCHECK cppcheck REQUIRED)
	if (${CMAKE_CXX_CPPCHECK} STREQUAL "cppcheck_path-NOTFOUND")
		message(FATAL_ERROR "cppcheck not found")
	endif()
	list(APPEND CMAKE_CXX_CPPCHECK
		"--enable=warning"
		"--force"
        "--suppress=*:${CMAKE_BINARY_DIR}/_deps/*") # Exclude all external dependencies
	message("CPPCheck Enabled: ${CMAKE_CXX_CPPCHECK}")
endif()

if (ENABLE_CLANG_TIDY)
    find_program(clang_tidy_path clang-tidy REQUIRED)
    if (${clang_tidy_path} STREQUAL "clang_tidy_path-NOTFOUND")
        message(FATAL_ERROR "clang-tidy not found")
    endif()
    set(CMAKE_CXX_CLANG_TIDY "${clang_tidy_path}")
    list(APPEND CMAKE_CXX_CLANG_TIDY
        "-header-filter=\"${CMAKE_SOURCE_DIRECTORY}\""
        "-checks=\"-*,clang-analyzer-*,modernize-*,clang-analyzer-*,cppcoreguidelines-*,cert-*,performance-*,portability-*,readability-*\""
	"-extra-arg=-Wno-unknown-warning-option")

    if (TYPECHECK_FIX_ERRORS)
        list(APPEND CMAKE_CXX_CLANG_TIDY "--fix")
    endif()
	message("Clang-Tidy Enabled: ${CMAKE_CXX_CLANG_TIDY}")
endif()

add_library(typecheck ${SRC_FILES} ${INC_FILES})
target_include_directories(typecheck PUBLIC include)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	target_precompile_headers(typecheck PUBLIC ${INC_FILES})
endif()

if (ENABLE_CPP_CHECK)
	set_property(TARGET typecheck PROPERTY CXX_CPPCHECK ${CMAKE_CXX_CPPCHECK})
	unset(CMAKE_CXX_CPPCHECK)
endif()

if (ENABLE_IWYU)
	set_property(TARGET typecheck PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path_and_options})
endif()

if (ENABLE_CLANG_TIDY)
	set_property(TARGET typecheck PROPERTY CXX_CLANG_TIDY ${CMAKE_CXX_CLANG_TIDY})
	set(CMAKE_CXX_CLANG_TIDY "")
endif()

target_compile_options(typecheck PRIVATE "${TYPECHECK_EXTRA_FLAGS}")

if (TYPECHECK_PRINT_DEBUG_CONSTRAINTS)
	message(STATUS "Typecheck: Debug Constraints Enabled")
    target_compile_definitions(typecheck PUBLIC "-DTYPECHECK_PRINT_DEBUG_CONSTRAINTS=0")
	if (TYPECHECK_PRINT_SHORT_DEBUG)
		message(STATUS "Typecheck: Short Debug Enabled")
    	target_compile_definitions(typecheck PUBLIC "-DTYPECHECK_PRINT_SHORT_DEBUG=0")
	endif()
endif()

target_compile_definitions(typecheck PUBLIC "$<$<CONFIG:Debug>:DEBUG>")
target_compile_definitions(typecheck PUBLIC "$<$<CONFIG:Release>:RELEASE>")
target_compile_definitions(typecheck PUBLIC "$<$<CONFIG:RelWithDebInfo>:DEBUG>")
target_compile_definitions(typecheck PUBLIC "$<$<CONFIG:MinSizeRel>:RELEASE>")

if(${TYPECHECK_ENABLE_BLOATY})
	set(bloatyBin $<TARGET_FILE:bloaty>)
	add_custom_target("typecheck_bloaty"
			COMMAND ${bloatyBin} $<TARGET_FILE:typecheck>
			DEPENDS bloaty typecheck
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Running Bloaty")
endif()

if (${TYPECHECK_BUILD_TESTS})
	if (NOT TARGET Catch2::Catch2)
		find_package(Catch QUIET)
	endif()

	if (NOT TARGET Catch2::Catch2)
		fetch_extern(catch2 https://github.com/catchorg/Catch2 ${CATCH_VERSION})
	endif()

    enable_testing()

	file(GLOB_RECURSE TEST_INC_FILES test/*.hpp)

	# Test everything (default test with `make test`
	add_executable(test_typecheck test/test_typecheck.cpp ${TEST_INC_FILES})
    target_link_libraries(test_typecheck typecheck Catch2::Catch2)
    target_include_directories(test_typecheck SYSTEM PUBLIC $<TARGET_PROPERTY:Catch2::Catch2,INTERFACE_INCLUDE_DIRECTORIES>)
	target_compile_definitions(test_typecheck PUBLIC "-DTEST_RESOLVERS")
	target_compile_definitions(test_typecheck PUBLIC "-DTEST_TYPE_MANAGER")
	target_compile_definitions(test_typecheck PUBLIC "-DTEST_TYPE_CONSTRAINTS")

	if (TYPECHECK_ENABLE_COVERAGE)
		setup_target_for_coverage_lcov(
			NAME coverage_typecheck
			EXECUTABLE $<TARGET_FILE:test_typecheck>
			LCOV_ARGS ${TYPECHECK_LCOV_ARGS}
			GENHTML_ARGS --legend --branch-coverage
		DEPENDENCIES test_typecheck)
	endif()
    add_test(typecheck test_typecheck)

	# Test just resolvers
	add_executable(test_resolvers test/test_resolvers.cpp ${TEST_INC_FILES})
    target_link_libraries(test_resolvers typecheck Catch2::Catch2)
    target_include_directories(test_resolvers SYSTEM PUBLIC $<TARGET_PROPERTY:Catch2::Catch2,INTERFACE_INCLUDE_DIRECTORIES>)
	target_compile_definitions(test_resolvers PUBLIC "-DTEST_RESOLVERS")
	if (TYPECHECK_ENABLE_COVERAGE)
		setup_target_for_coverage_lcov(
			NAME coverage_resolvers
			EXECUTABLE $<TARGET_FILE:test_resolvers>
			LCOV_ARGS ${TYPECHECK_LCOV_ARGS}
			GENHTML_ARGS --legend --branch-coverage
			DEPENDENCIES test_resolvers)
	endif()

	# Test just constraints
	add_executable(test_constraints test/test_constraints.cpp ${TEST_INC_FILES})
    target_link_libraries(test_constraints typecheck Catch2::Catch2)
    target_include_directories(test_constraints SYSTEM PUBLIC $<TARGET_PROPERTY:Catch2::Catch2,INTERFACE_INCLUDE_DIRECTORIES>)
	target_compile_definitions(test_constraints PUBLIC "-DTEST_TYPE_CONSTRAINTS")
	if (TYPECHECK_ENABLE_COVERAGE)
		setup_target_for_coverage_lcov(
			NAME coverage_constraints
			EXECUTABLE $<TARGET_FILE:test_constraints>
			LCOV_ARGS ${TYPECHECK_LCOV_ARGS}
			GENHTML_ARGS --legend --branch-coverage
			DEPENDENCIES test_constraints)
	endif()

	# Test just type manager
	add_executable(test_type_manager test/test_type_manager.cpp ${TEST_INC_FILES})
    target_link_libraries(test_type_manager typecheck Catch2::Catch2)
    target_include_directories(test_type_manager SYSTEM PUBLIC $<TARGET_PROPERTY:Catch2::Catch2,INTERFACE_INCLUDE_DIRECTORIES>)
	target_compile_definitions(test_type_manager PUBLIC "-DTEST_TYPE_MANAGER")
	if (TYPECHECK_ENABLE_COVERAGE)
		setup_target_for_coverage_lcov(
			NAME coverage_type_manager
			EXECUTABLE $<TARGET_FILE:test_type_manager>
			LCOV_ARGS ${TYPECHECK_LCOV_ARGS}
			GENHTML_ARGS --legend --branch-coverage
			DEPENDENCIES test_type_manager)
	endif()

	# Test just raw objects
	add_executable(test_obj test/test_obj.cpp ${TEST_INC_FILES})
    target_link_libraries(test_obj typecheck Catch2::Catch2)
    target_include_directories(test_obj SYSTEM PUBLIC $<TARGET_PROPERTY:Catch2::Catch2,INTERFACE_INCLUDE_DIRECTORIES>)
	target_compile_definitions(test_obj PUBLIC "-DTEST_TYPE_MANAGER")
	if (TYPECHECK_ENABLE_COVERAGE)
		setup_target_for_coverage_lcov(
			NAME coverage_obj
			EXECUTABLE $<TARGET_FILE:test_obj>
			LCOV_ARGS ${TYPECHECK_LCOV_ARGS}
			GENHTML_ARGS --legend --branch-coverage
			DEPENDENCIES test_obj)
	endif()
endif()

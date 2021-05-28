cmake_minimum_required(VERSION 3.3)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")

project(ktest)

set(KTEST_MAX_TESTS 1024 CACHE STRING "Max tests : uint (static array size)")
set(KTEST_FAIL_RETURN_CODE 1 CACHE STRING "Return code on test failure : int")
set(KTEST_MULTITHREADED true CACHE STRING "Whether ktest::main uses multiple threads : bool")

set(INCLUDES include/ktest/ktest.hpp)
set(SOURCES src/ktest.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${INCLUDES} ${SOURCES})

add_library(ktest ${INCLUDES} ${SOURCES})
add_library(ktest::ktest ALIAS ktest)
target_compile_definitions(ktest PRIVATE KTEST_MAX_TESTS=${KTEST_MAX_TESTS} KTEST_FAIL_RETURN_CODE=${KTEST_FAIL_RETURN_CODE})
target_compile_features(ktest PUBLIC cxx_std_17)
target_include_directories(ktest PUBLIC include PRIVATE src)

add_library(ktest-main src/main_lib.cpp)
add_library(ktest::main ALIAS ktest-main)
target_compile_definitions(ktest-main PRIVATE KTEST_MULTITHREADED=${KTEST_MULTITHREADED})
target_link_libraries(ktest-main PUBLIC ktest::ktest)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(dtasks PRIVATE /MP)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(ktest PRIVATE -Wextra -Wall -Werror=return-type $<$<NOT:$<CONFIG:Debug>>:-Werror>)
endif()

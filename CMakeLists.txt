cmake_minimum_required(VERSION 3.3)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")

project(dtest)

set(DTEST_FAIL_RETURN_CODE 1 CACHE STRING "Return code on test failure : int")
set(DTEST_MULTITHREADED true CACHE STRING "Whether dtest::main uses multiple threads : bool")

set(INCLUDES include/dumb_test/dtest.hpp)
set(SOURCES src/dtest.cpp)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${INCLUDES} ${SOURCES})

add_library(dtest ${INCLUDES} ${SOURCES})
add_library(dtest::dtest ALIAS dtest)
target_compile_definitions(dtest PRIVATE DTEST_FAIL_RETURN_CODE=${DTEST_FAIL_RETURN_CODE})
target_compile_features(dtest PUBLIC cxx_std_17)
target_include_directories(dtest PUBLIC include PRIVATE src)

add_library(dtest-main src/main_lib.cpp)
add_library(dtest::main ALIAS dtest-main)
target_compile_definitions(dtest-main PRIVATE DTEST_MULTITHREADED=${DTEST_MULTITHREADED})
target_link_libraries(dtest-main PUBLIC dtest::dtest)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(dtest PRIVATE /MP)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(dtest PRIVATE -Wextra -Wall -Werror=return-type $<$<NOT:$<CONFIG:Debug>>:-Werror>)
endif()

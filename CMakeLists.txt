cmake_minimum_required(VERSION 3.14)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")
set(CMAKE_DEBUG_POSTFIX "-d")

project(dtest)

set(DTEST_FAIL_RETURN_CODE 1 CACHE STRING "Return code on test failure : int")
set(DTEST_MULTITHREADED true CACHE STRING "Whether dtest::main uses multiple threads : bool")

# cmake-utils
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
  cmake-utils
  GIT_REPOSITORY https://github.com/karnkaul/cmake-utils
  GIT_TAG v1.0
)
FetchContent_MakeAvailable(cmake-utils)
FetchContent_GetProperties(cmake-utils)

set(${PROJECT_NAME}_version 1.0.0)
set(${PROJECT_NAME}_soversion 1)
add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME} PROPERTY VERSION ${${PROJECT_NAME}_version})
set_property(TARGET ${PROJECT_NAME} PROPERTY SOVERSION ${${PROJECT_NAME}_soversion})
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_${PROJECT_NAME}_MAJOR_VERSION ${${PROJECT_NAME}_soversion})
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ${PROJECT_NAME}_MAJOR_VERSION)

target_compile_definitions(dtest PRIVATE DTEST_FAIL_RETURN_CODE=${DTEST_FAIL_RETURN_CODE})
target_compile_features(dtest PUBLIC cxx_std_17)
target_include_directories(dtest PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)
target_sources(${PROJECT_NAME} PRIVATE
  include/dumb_test/detail/operators.hpp
  include/dumb_test/dtest.hpp
  src/dtest.cpp
)
get_target_property(sources ${PROJECT_NAME} SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(dtest PRIVATE /MP)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(dtest PRIVATE -Wextra -Wall -Werror=return-type $<$<NOT:$<CONFIG:Debug>>:-Werror>)
endif()

add_library(dtest-main src/main_lib.cpp)
add_library(dtest::main ALIAS dtest-main)
target_compile_definitions(dtest-main PRIVATE DTEST_MULTITHREADED=${DTEST_MULTITHREADED})
target_link_libraries(dtest-main 
  PUBLIC
    dtest::dtest
  PRIVATE
    $<$<STREQUAL:${CMAKE_SYSTEM_NAME},Linux>:pthread>
)

include("${cmake-utils_SOURCE_DIR}/cmake-utils.cmake")
install_and_export_target(${PROJECT_NAME})
install_target(${PROJECT_NAME}-main ${PROJECT_NAME})
install_pkg_config(${PROJECT_NAME}-main)
install_pkg_version(${PROJECT_NAME}-main ${${PROJECT_NAME}_version})
export_target_to_build_tree(${PROJECT_NAME}-main ${PROJECT_NAME})

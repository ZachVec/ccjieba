cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(ccjieba LANGUAGES CXX)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wshadow -Werror=return-type)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include_directories( ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/include)

file(GLOB_RECURSE PROJECT_SRCS ${PROJECT_SOURCE_DIR}/src/*.cc)

option(BUILD_SHARED_LIBS "Build shared library (ON) or static library (OFF)" OFF)

add_library(${PROJECT_NAME} ${PROJECT_SRCS})

if(BUILD_SHARED_LIBS)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION 1.0.0
        SOVERSION 1
    )
endif()

set(DATA_ROOT "${CMAKE_SOURCE_DIR}/data")
add_compile_definitions(DATA_ROOT="${DATA_ROOT}")

set(TEST_DATA_ROOT "${CMAKE_SOURCE_DIR}/tests/data")
add_compile_definitions(TEST_DATA_ROOT="${TEST_DATA_ROOT}")

add_executable(demo demo.cc)
target_link_libraries(demo PRIVATE ${PROJECT_NAME})

option(BUILD_TESTING "Build the testing tree" ON)
if(BUILD_TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
        URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    file(GLOB_RECURSE PROJECT_TEST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/tests/*_test.cc)
    add_executable(unittest ${PROJECT_TEST_SOURCE_DIR})
    target_link_libraries(unittest PRIVATE ${PROJECT_NAME} GTest::gtest GTest::gtest_main)
    target_include_directories(unittest PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/include)

    include(GoogleTest)
    gtest_discover_tests(unittest)

endif()



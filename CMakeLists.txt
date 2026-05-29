cmake_minimum_required(VERSION 3.14)
project(rvc_control_sw LANGUAGES CXX)

# NFR-BUILD-01: C++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# -----------------------------------------------------------------------
# rvc_core: core logic + app adapters (everything except main.cpp)
# -----------------------------------------------------------------------
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_library(rvc_core STATIC ${LIB_SOURCES})
target_include_directories(rvc_core PUBLIC include)

# -----------------------------------------------------------------------
# oop: main executable
# -----------------------------------------------------------------------
add_executable(oop src/app/main.cpp)
target_link_libraries(oop PRIVATE rvc_core)

# -----------------------------------------------------------------------
# Tests (GoogleTest via FetchContent — no pre-install required)
# -----------------------------------------------------------------------
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Unit tests
file(GLOB UT_SOURCES CONFIGURE_DEPENDS test/unit/*.cpp)
if(UT_SOURCES)
    add_executable(oop_test ${UT_SOURCES})
    target_include_directories(oop_test PRIVATE test/unit/stub include)
    target_link_libraries(oop_test PRIVATE rvc_core GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(oop_test)
endif()

# System tests (can use both stub and simulator headers)
file(GLOB ST_SOURCES CONFIGURE_DEPENDS test/system/*.cpp)
if(ST_SOURCES)
    add_executable(oop_system_test ${ST_SOURCES})
    target_include_directories(oop_system_test PRIVATE test/unit/stub test/simulator include)
    target_link_libraries(oop_system_test PRIVATE rvc_core GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(oop_system_test)
endif()

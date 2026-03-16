cmake_minimum_required(VERSION 3.14)
project(oop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 메인 실행파일
add_executable(oop src/main.cpp)

# Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

# 테스트 실행파일
enable_testing()
add_executable(oop_test test/test_main.cpp)
target_link_libraries(oop_test gtest_main)
add_test(NAME oop_test COMMAND oop_test)
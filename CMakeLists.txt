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
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# 테스트 설정
enable_testing()
add_executable(oop_test test/test_main.cpp)

target_link_libraries(oop_test GTest::gtest_main)

add_test(NAME oop_test COMMAND oop_test)
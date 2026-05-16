cmake_minimum_required(VERSION 3.14)
project(oop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_COVERAGE "Enable gcov/lcov coverage" OFF)
if(ENABLE_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

set(LIB_SOURCES
    src/FrontSensor.cpp
    src/LeftSensor.cpp
    src/RightSensor.cpp
    src/DustSensor.cpp
    src/MotorController.cpp
    src/CleanerController.cpp
    src/RVCOrchestrator.cpp
    src/DigitalClock.cpp
)

set(TEST_SOURCES
    test/RVCOrchestratorTest.cpp
    test/MotorControllerTest.cpp
    test/CleanerControllerTest.cpp
    test/DigitalClockTest.cpp
)

# 메인 실행파일
add_executable(oop src/main.cpp ${LIB_SOURCES})
target_include_directories(oop PRIVATE include)

# Google Test
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# 단위 테스트
enable_testing()
add_executable(oop_test ${TEST_SOURCES} ${LIB_SOURCES})
target_include_directories(oop_test PRIVATE include test)
target_link_libraries(oop_test GTest::gtest_main)

add_test(NAME oop_test COMMAND oop_test)

# System Tests — 추후 추가 예정
# add_executable(oop_system_test test/system/RVCSystemTest.cpp ${LIB_SOURCES})
# target_include_directories(oop_system_test PRIVATE include test)
# add_test(NAME oop_system_test COMMAND oop_system_test --xml=${CMAKE_BINARY_DIR}/system_test_results.xml)

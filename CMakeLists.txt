cmake_minimum_required(VERSION 3.14)
project(oop)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_SOURCES
    src/hardware/Motor.cpp
    src/hardware/Cleaner.cpp
    src/hardware/Sensor.cpp
    src/hardware/FrontSensor.cpp
    src/hardware/LeftSensor.cpp
    src/hardware/RightSensor.cpp
    src/hardware/DustSensor.cpp
    src/controller/RVCPowerController.cpp
    src/controller/MotorController.cpp
    src/controller/CleanerController.cpp
    src/controller/MovementPolicyController.cpp
    src/controller/CleaningPolicyController.cpp
    src/handler/ErrorInfo.cpp
    src/handler/ErrorHandler.cpp
    src/handler/CLIHandler.cpp
    src/RVCOrchestrator.cpp
)

set(TEST_SOURCES
    test/handler/ErrorInfoTest.cpp
    test/handler/CLIHandlerTest.cpp
    test/handler/ErrorHandlerTest.cpp
    test/controller/RVCPowerControllerTest.cpp
    test/controller/MovementPolicyControllerTest.cpp
    test/controller/MotorControllerTest.cpp
    test/controller/CleanerControllerTest.cpp
    test/hardware/CleanerTest.cpp
    test/controller/CleaningPolicyControllerTest.cpp
    test/RVCOrchestratorTest.cpp
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


# 테스트 설정
enable_testing()
add_executable(oop_test ${TEST_SOURCES} ${LIB_SOURCES})
target_include_directories(oop_test PRIVATE include test)
target_link_libraries(oop_test GTest::gtest_main)

add_test(NAME oop_test COMMAND oop_test)

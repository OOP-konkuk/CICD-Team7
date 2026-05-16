cmake_minimum_required(VERSION 3.14)
project(ooi_unit_tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google Test (FetchContent)
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 소스 파일 경로 (04-ooi/code/src, include)
set(OOI_SRC_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/../code/src)
set(OOI_INC_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/../code/include)
set(STUB_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/stubs)

set(OOI_SOURCES
    ${OOI_SRC_DIR}/FrontSensor.cpp
    ${OOI_SRC_DIR}/LeftSensor.cpp
    ${OOI_SRC_DIR}/RightSensor.cpp
    ${OOI_SRC_DIR}/DustSensor.cpp
    ${OOI_SRC_DIR}/MotorController.cpp
    ${OOI_SRC_DIR}/CleanerController.cpp
    ${OOI_SRC_DIR}/RVCOrchestrator.cpp
    ${OOI_SRC_DIR}/DigitalClock.cpp
)

set(TEST_SOURCES
    RVCOrchestratorTest.cpp
    MotorControllerTest.cpp
    CleanerControllerTest.cpp
)

# 테스트 실행파일
add_executable(ooi_unit_tests ${TEST_SOURCES} ${OOI_SOURCES})

target_include_directories(ooi_unit_tests PRIVATE
    ${OOI_INC_DIR}
    ${STUB_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ooi_unit_tests GTest::gtest_main)

# CTest 등록 (로컬: ctest, 서버: GitHub Actions ctest)
enable_testing()
include(GoogleTest)
gtest_discover_tests(ooi_unit_tests)

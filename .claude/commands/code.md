---
description: SDD를 따라 C++17 구현(include/, src/, main.cpp, CMakeLists.txt)을 작성하고 WSL에서 빌드를 검증한다. 디렉토리 구조·파일명·클래스명은 모두 SDD가 정의한 것을 따른다.
---

# /code — C++ 구현

> **정책 (CLAUDE.md 2·6절)**
> 구현은 **SDD를 진실 소스**로 한다. SDD가 정의하지 않은 클래스/파일을 임의로 만들지 않는다.
> PDF·이전 학기 코드(`reference/CICD-Team7-main/`)의 디렉토리 구조·헤더 이름을 답습하지 않는다 — 본 단계의 입력에 포함되지 않는다.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /code" > log/${TS}_code.md
echo "" >> log/${TS}_code.md
echo "## 사용 프롬프트" >> log/${TS}_code.md
echo "\`/code\`" >> log/${TS}_code.md
```

## 1. 입력 (이외 자료 보지 말 것)

- `docs/sdd/SDD.md` (단일 진실 소스 — 디렉토리·파일·클래스명·시그니처 모두 여기서 따옴)
- `docs/srs/SRS.md` (사후조건·검증 기준 확인용)

`reference/`, `docs/specs/`는 본 단계에서 열지 않는다.

## 2. 산출물

### A. 헤더·구현 (`include/`, `src/`)

**SDD 8절 "디렉토리·파일 매핑" 표를 그대로 구현한다.** 본 커맨드는 어떤 헤더가 어디 들어가야 하는지를 강제하지 않는다 — SDD가 결정한 구조를 따른다.

만약 SDD가 어느 부분에서 모호하다면 (예: 헤더 분할 단위) 본 커맨드에서 임의로 결정하지 말고 `/sdd`로 돌아가 SDD를 보강하고 다시 진행한다. 코드는 SDD에서 흘러내려야 한다 (역방향 금지).

### B. main (`src/main.cpp`)

- SDD가 정의한 컴포지션 루트에서 의존성을 조립하고 시스템을 실행.
- 표준 출력·표준 입력 사용은 SDD가 정한 통제점(예: 단일 입출력 핸들러)을 거쳐야 한다.

### C. 빌드 (`CMakeLists.txt`)

다음 골격을 베이스로 하되, **SDD 9절 "빌드 설계"가 다른 구조를 정의했다면 SDD를 따른다.**

```cmake
cmake_minimum_required(VERSION 3.14)
project(rvc_control_sw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)

# 라이브러리 소스 (main.cpp 제외)
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_library(rvc_core STATIC ${LIB_SOURCES})
target_include_directories(rvc_core PUBLIC include)

# 메인 실행파일
add_executable(oop src/main.cpp)
target_link_libraries(oop PRIVATE rvc_core)

# 테스트
enable_testing()
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

file(GLOB UT_SOURCES CONFIGURE_DEPENDS test/unit/*.cpp)
if(UT_SOURCES)
    add_executable(oop_test ${UT_SOURCES})
    target_link_libraries(oop_test PRIVATE rvc_core GTest::gtest_main)
    add_test(NAME unit_tests COMMAND oop_test)
endif()

file(GLOB ST_SOURCES CONFIGURE_DEPENDS test/system/*.cpp test/simulator/*.cpp)
if(ST_SOURCES)
    add_executable(oop_system_test ${ST_SOURCES})
    target_include_directories(oop_system_test PRIVATE test/simulator)
    target_link_libraries(oop_system_test PRIVATE rvc_core GTest::gtest_main)
    add_test(NAME system_tests COMMAND oop_system_test)
endif()
```

## 3. 구현 규칙

- C++17 표준 엄수, `-Wall -Wextra -Wpedantic` 경고 0.
- 헤더 가드: `#pragma once`.
- RAII, smart pointer 우선. 원시 포인터는 비소유 의존성에 한해 허용.
- 의존성 주입: 생성자로 추상화 포인터/참조 받기. 구체 클래스 간 직접 결합 금지.
- 표준 출력 메시지는 SDD가 정한 단일 책임 통제점을 통해서만.
- **PDF/이전 코드의 클래스명·메서드명을 우연이라도 그대로 가져오지 않는다.** SDD가 정의한 이름만 사용.

## 4. 빌드 검증 (WSL)

```bash
wsl bash -lc "cd '$(pwd)' && cmake -S . -B build && cmake --build build -j 2>&1 | tee /tmp/rvc_build.log"
```

빌드 실패 시 즉시 수정. 경고도 0이어야 함. 검증이 끝날 때까지 본 단계는 완료가 아니다.

빌드 오류가 SDD의 모호함에서 비롯되면, 코드를 임시방편으로 고치지 말고 `/sdd`로 돌아가 SDD를 보강한 뒤 다시 `/code`를 진행한다.

## 5. 자기 점검

- [ ] 모든 소스·헤더 파일이 SDD 8절 매핑 표에 등장하는가? (SDD에 없는 임의 파일을 만들지 않았는가?)
- [ ] 모든 클래스명·메서드명·인터페이스명이 SDD에 정의된 그대로인가?
- [ ] PDF Domain Model/Sequence Diagram의 이름(RVCOrchestrator·MovementPolicyController·startCleaning·moveForward 등)을 우연히 차용하지 않았는가?
- [ ] 빌드 경고 0개, 모든 단위 테스트가 (있다면) 통과하는가?
- [ ] `reference/`·`docs/specs/`를 본 단계에서 한 번도 열지 않았는가?

## 6. 로그 종료

`log/${TS}_code.md`에 채움:
- `## 주요 결정`: 의존성 주입 방식, 소유권 정책, CMake 구조 (SDD 9절과 차이가 있다면 그 근거), SDD 모호함을 발견해 보강한 항목
- `## 생성·수정 파일`: 모든 .h/.cpp/CMakeLists.txt 경로
- `## 결과 요약`: 헤더/소스 파일 수, LOC, 빌드 시간, 빌드 경고 수(0이어야 함)
- `## 검증 방법`: 위의 cmake 명령
- `## 다음 단계`: `/ut`

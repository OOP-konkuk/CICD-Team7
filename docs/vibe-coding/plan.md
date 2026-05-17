# RVC Control SW — Vibe Coding Plan

> **Branch**: vibe-coding  
> **언어**: C++17 / CMake  
> **CI/CD**: GitHub Actions (build-test, clang-tidy, cppcheck, sonarcloud)

---

## 전체 작업 단계 및 진척도

| # | 단계 | 상태 | 산출물 위치 |
|---|------|------|------------|
| 1 | Use Cases | ✅ 완료 | `docs/vibe-coding/01-inception/use-cases.mdx` |
| 2 | Use Case Diagram | ✅ 완료 | `docs/vibe-coding/01-inception/images/RVC_UseCaseDiagram.png` |
| 3 | System Sequence Diagram (SSD) | ✅ 완료 | `docs/vibe-coding/02-ooa/ssd.mdx` |
| 4 | Domain Model | ✅ 완료 | `docs/vibe-coding/02-ooa/domain-model.mdx` |
| 5 | Sequence Diagram (SD) | ✅ 완료 | `docs/vibe-coding/03-ood/sequence-diagrams.mdx` |
| 6 | Class Diagram | ✅ 완료 | `docs/vibe-coding/03-ood/class-diagram.mdx` |
| 7 | 구현 (OOI) | ✅ 완료 | `src/*.cpp` / `include/*.h` |
| 8 | Unit Test | ✅ 완료 | `test/*Test.cpp` |
| 9 | System Test + 시뮬레이터 | ✅ 완료 | `test/system/` / `test/simulator/` |
| 10 | Static Code Analysis | ⬜ 미완료 | CI 워크플로 설정 완료, 리포트 수집 필요 |

---

## 단계별 상세 내역

### 1. Use Cases ✅

- **범위**: UC-1~UC-4 (Perform Cleaning, Avoid Front Obstacle, Avoid All Obstacles, Power Up Cleaning)
- **Actor**: Digital Clock (Hidden), Front/Left/Right Sensor, Dust Sensor
- **출력**: Functional Requirement → Use Case 매핑표 + 상세 시나리오

### 2. Use Case Diagram ✅

- PlantUML 소스 포함, `<<extend>>` 관계 (UC-2 → UC-3) 표현
- 생성 이미지: `RVC_UseCaseDiagram.png`

### 3. System Sequence Diagram ✅

- SSD-1~SSD-4 (각 UC 1:1 대응)
- System Operations 도출: `onTick()`, `onFrontDetected()`
- Traceability 표 포함

### 4. Domain Model ✅

- 개념 클래스: RVCOrchestrator, Sensor(abstract), FrontSensor, LeftSensor, RightSensor, DustSensor, MotorController, CleanerController, DigitalClock
- 열거형: Direction (FORWARD/BACKWARD/LEFT/RIGHT), CleanMode (ON/OFF/UP)

### 5. Sequence Diagram ✅

- SD-1~SD-4 (SSD `:System` 블랙박스를 설계 클래스로 구체화)
- SD-4: `loop update()=false` alt 프래그먼트 포함

### 6. Class Diagram ✅

- OOD 설계 클래스 다이어그램 (가시성, 타입, 메서드 시그니처)
- `CleanerController`: 타이머 속성 (`boostDurationMs`, `boostEndTime`, `update()`) 추가
- `DigitalClock`: `intervalMs`, `start()`, `stop()`, `tick()` 추가

### 7. 구현 (OOI) ✅

```
include/
├── types.h               Direction, CleanMode 열거형
├── Sensor.h              추상 클래스 (virtual detect())
├── FrontSensor.h / .cpp
├── LeftSensor.h / .cpp
├── RightSensor.h / .cpp
├── DustSensor.h / .cpp
├── MotorController.h / .cpp
├── CleanerController.h / .cpp   타이머 (boostDurationMs, update())
├── DigitalClock.h / .cpp        스레드 기반 주기 tick
└── RVCOrchestrator.h / .cpp     System Operations 구현
```

- `RVCOrchestrator::onTick()` — SD-1/SD-4 분기
- `RVCOrchestrator::onFrontDetected()` — SD-2/SD-3 분기
- `CleanerController::update()` — steady_clock 기반 boost 만료 확인

### 8. Unit Test ✅

| 파일 | 테스트 대상 | 주요 내용 |
|------|------------|---------|
| `RVCOrchestratorTest.cpp` | RVCOrchestrator | onTick/onFrontDetected 경로 검증 (StubSensor) |
| `MotorControllerTest.cpp` | MotorController | setDirection/getDirection |
| `CleanerControllerTest.cpp` | CleanerController | setMode/getMode/update/boost 타이머 |
| `DigitalClockTest.cpp` | DigitalClock | tick()/start()/stop() 스레드 동작 |

- 프레임워크: Google Test
- Stub: `test/stubs/StubSensors.h` (StubFrontSensor 등 detect() 반환값 주입)

### 9. System Test + 시뮬레이터 ✅

#### 시뮬레이터 (`test/simulator/RVCSimulator.h`)

현재 구현(onTick/onFrontDetected API)에 맞게 재작성.

```
RVCSystemSimulator
├── StubSensor 4종 (환경 주입)
├── MotorController / CleanerController{20ms}
├── RVCOrchestrator (실제 구현체)
├── tick()          → onTick()
├── frontDetected() → onFrontDetected()
├── set*Obstacle() / setDustDetected()
└── motorIs*() / cleanerIs*() (상태 조회)
```

#### 시스템 테스트 (`test/system/RVCSystemTest.cpp`)

프레임워크: `SimpleTestRunner` (JUnit XML 출력 지원)

| Flow | UC | 케이스 수 | 검증 내용 |
|------|----|----------|---------|
| Flow1_PerformCleaning | UC-1 | 10 | tick + 장애물 없음 → FORWARD + ON |
| Flow2_AvoidFrontObstacle | UC-2 | 8 | frontDetected + 한쪽 열림 → FORWARD + ON |
| Flow3_AvoidAllObstacles | UC-3 | 8 | frontDetected + 양방향 막힘 → FORWARD + ON |
| Flow4_PowerUpCleaning | UC-4 | 10 | tick + 먼지 감지 → boost → ON |
| Flow5_Integration | UC-1~4 복합 | 9 | 순서대로 UC 발생하는 멀티 시나리오 |
| **합계** | | **45** | |

**결과**: `ctest` — 단위(2/2) + 시스템(45/45) 전체 통과

### 10. Static Code Analysis ⬜

CI 워크플로는 설정 완료. 아래 항목이 남아있음:

| 도구 | 워크플로 파일 | 상태 |
|------|--------------|------|
| cppcheck | `.github/workflows/cppcheck.yml` | CI 트리거만 설정 |
| clang-tidy | `.github/workflows/clang-tidy.yml` | CI 트리거만 설정 |
| SonarCloud | `.github/workflows/sonarcloud.yml` | CI 트리거만 설정 |

**남은 작업**: develop 브랜치에 PR 머지 후 CI 실행 → 리포트 확인 → 지적 이슈 수정

---

## 파일 구조 요약

```
CICD-Team7/
├── include/              헤더 (클래스 구조)
├── src/                  구현 (.cpp)
├── test/
│   ├── stubs/            StubSensors.h
│   ├── simulator/        RVCSimulator.h  ← 재작성 완료
│   ├── system/           RVCSystemTest.cpp / SimpleTestRunner.h  ← 재작성 완료
│   ├── RVCOrchestratorTest.cpp
│   ├── MotorControllerTest.cpp
│   ├── CleanerControllerTest.cpp
│   └── DigitalClockTest.cpp
├── docs/vibe-coding/
│   ├── 01-inception/     Use Cases + Diagram
│   ├── 02-ooa/           SSD + Domain Model
│   ├── 03-ood/           SD + Class Diagram
│   ├── 04-ooi/           Implementation 문서
│   └── plan.md           ← 이 파일
├── CMakeLists.txt        시스템 테스트 타겟 활성화 완료
└── .github/workflows/    CI (build-test, cppcheck, clang-tidy, sonarcloud)
```

---

## 다음 작업: Static Code Analysis

1. vibe-coding → develop PR 머지
2. CI 자동 실행 (cppcheck / clang-tidy / sonarcloud)
3. 리포트 확인 후 지적 이슈 수정
4. 수정 사항 커밋 → develop 머지

---

*최종 업데이트: 2026-05-17*

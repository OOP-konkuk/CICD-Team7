# 통합 Diff 매트릭스 — legacy 대비 변경/일치 (오른쪽 근접 센서 삭제)

> 본 문서는 `/change-impact`~`/sa` 각 단계 로그의 `## 차이 표기`를 한 곳에 통합한 것이다.
> 본 과제 채점의 핵심: **무엇이 바뀌었고(변경) 무엇이 그대로인지(일치)** 를 빠짐없이 보인다.
> 기계적 근거: `diff -rq legacy/<dir> <dir>` (재현 명령은 §4).
> 작성: 2026-05-29 `/package`

## 0. 한눈 집계

| 구분 | 파일 수 | 비고 |
|---|---|---|
| **변경 (MODIFY)** | **18** | 문서 8 + 코드 6 + 테스트 5 (− 중복 제외) → 아래 상세 |
| **일치 (KEEP)** | **다수** | 인터페이스 6, stub 6, sim 6, 비센서 구현/헤더 다수, CMakeLists 등 |
| 신규 (NEW) | 2 | `docs/maintenance/impact.md`, `docs/maintenance/diff-matrix.md` |

변경 18파일 상세: SRS 1 · SDD 1 · specs 3 · SA 3 · include 3 · src 3 · simulator 1 · system 2 · unit 2 = **19** 항목(SA 3건은 재생성·도구 리포트).
핵심 코드/테스트/문서 기준 변경은 **16파일**(SA 3 제외).

---

## 1. 문서 (docs/)

| 아티팩트 | 파일 | legacy 대비 | 변경 요지 / 일치 근거 |
|---|---|---|---|
| SRS | `docs/srs/SRS.md` | **변경** | 센서 4종→3종, 컨텍스트/액터에서 우측 근접 센서 노드·행 삭제, FR-MOVE-02 방향 결정 표에서 우측 열·우회전 행 삭제 후 "좌 가능→좌회전 / 좌 막힘→후진" 단순화, UC-03 주액터·흐름 갱신. **우회전(모터 출력)·`MotorCommand::TURN_RIGHT`는 존속**(삭제된 것은 우측 센서 입력) |
| SDD | `docs/sdd/SDD.md` | **변경** | 매핑 표 IProximitySensor 3→2 인스턴스, `CleaningEngine`/`ObstacleHandler` ctor의 `rightSensor`/`right_` 제거, 회피 시퀀스에서 우측 센서 조회·우회전 분기 제거, `AvoidPath::TURN_RIGHT` 제거. `MotorCommand::TURN_RIGHT`·`executeBackAndTurn()`는 일치 |
| specs | `docs/specs/requirements.md` | **변경** | "좌·우측 센서 조회"→"좌측", 결정 표 우측 열 제거 (TP#5 중간 산출물, `/srs`에서 일관 갱신) |
| specs | `docs/specs/system-overview.md` | **변경** | 컨텍스트 다이어그램 우측 센서 노드 제거 |
| specs | `docs/specs/use-cases.md` | **변경** | UC 결정 표 우측 열·"Turn Right" 매핑 제거 |
| SA | `docs/sa/cppcheck.txt` | **변경(재생성)** | 수정 코드 기준 GCC 진단 재실행 — 0 warnings (legacy 일치) |
| SA | `docs/sa/clang-tidy.txt` | **변경(재생성)** | -fanalyzer 재실행 — 프로젝트 이슈 0 (legacy 일치) |
| SA | `docs/sa/summary.md` | **변경(재생성)** | legacy 대비 비교 추가: 이슈 0→0 일치, STL 거짓 양성 5→0은 toolchain 차이 |

---

## 2. 코드 (include/ · src/ · CMakeLists.txt)

### 2.1 변경

| 파일 | legacy 대비 | 변경 요지 |
|---|---|---|
| `include/common/types.hpp` | **변경** | `AvoidPath::TURN_RIGHT` 멤버 제거. `MotorCommand::TURN_RIGHT`는 유지(모터 명령) |
| `include/core/cleaning_engine.hpp` | **변경** | ctor `rightSensor` 인자 제거(8→7 인자), `right_` 멤버 제거 |
| `include/core/obstacle_handler.hpp` | **변경** | ctor `right` 인자·`right_` 멤버 제거 |
| `src/core/cleaning_engine.cpp` | **변경** | ctor 초기화에서 `rightSensor`/`right_` 제거, `obstacleHandler_(leftSensor, …)` |
| `src/core/obstacle_handler.cpp` | **변경** | `selectPath()`의 우측 분기(`if(!right_.isBlocked()) return TURN_RIGHT`) 제거, `avoidAndReturn()`의 `AvoidPath::TURN_RIGHT` case 제거. `executeBackAndTurn()`(좌 센서만 사용)은 유지 |
| `src/app/main.cpp` | **변경** | `DemoSensor right` 인스턴스·`engine(...,right,...)` 전달 제거. `DemoDriveTrain`의 "TURN_RIGHT" 라벨 문자열은 유지(모터 명령) |

### 2.2 일치 (KEEP — 우측 센서 무관)

| 파일 | 근거 |
|---|---|
| `include/interface/i_proximity_sensor.hpp` | `isBlocked()` 방향 비특정 단일 인터페이스 — 인스턴스 수만 줄 뿐 인터페이스 불변 |
| `include/interface/{i_dust_sensor,i_drive_train,i_cleaning_unit,i_output_log,i_command_source}.hpp` | 센서 무관 |
| `include/core/clean_intensity_controller.hpp`, `src/core/clean_intensity_controller.cpp` | 청소 강도 — 센서 무관 |
| `include/app/{console_log,cli_command_source}.hpp`, `src/app/{console_log,cli_command_source}.cpp` | I/O 어댑터 — 센서 무관 |
| `CMakeLists.txt` | 소스 glob 방식, 센서별 항목 없음 → **diff 없음(완전 일치)** |

---

## 3. 테스트 (test/)

### 3.1 변경

| 파일 | legacy 대비 | 변경 요지 |
|---|---|---|
| `test/unit/test_obstacle_handler.cpp` | **변경** | 픽스처/ctor에서 `right` stub·인자 제거, `TurnRightWhenLeftBlockedRightClear` 테스트 삭제, 우측 polling 카운트 검증 제거, BackPath 기대값 조정 |
| `test/unit/test_cleaning_engine.cpp` | **변경** | 픽스처/ctor `(front,left,right,…)`→`(front,left,…)` |
| `test/simulator/rvc_sim_harness.hpp` | **변경** | `right_` 멤버·`engine_(…,right_,…)` 배선 제거, `setRightBlocked`/`injectRightObstacle`/`rightSensorCalls`/`rightSensor()` API 제거 |
| `test/system/test_system_scenarios.cpp` | **변경** | 픽스처/오류 시나리오에서 `right` 제거, `ObstacleAvoidanceRightPath` 시나리오 삭제, BackPath 트리거를 "좌측 막힘"으로 단순화 |
| `test/system/test_uc_scenarios.cpp` | **변경** | `RightPath_WhenLeftBlockedRightClear` 삭제, `BackPath_WhenAllBlocked`→`BackPath_WhenLeftBlocked`(`setRightBlocked` 제거), UC-06 오류 시나리오 우측 인자 제거 |

### 3.2 일치 (KEEP)

| 파일 | 근거 |
|---|---|
| `test/unit/test_clean_intensity_controller.cpp` | 센서 무관 |
| `test/unit/stub/stub_proximity_sensor.hpp` | 방향 비특정 — 인스턴스 수만 테스트에서 변동 |
| `test/unit/stub/{stub_dust_sensor,stub_drive_train,stub_cleaning_unit,stub_output_log,stub_command_source}.hpp` | 센서 무관 |
| `test/simulator/sim_proximity_sensor.hpp` | 방향 비특정 |
| `test/simulator/{sim_dust_sensor,sim_drive_train,sim_cleaning_unit,sim_output_log,sim_command_source}.hpp` | 센서 무관 |

---

## 4. 기계적 재현 (diff -rq)

```bash
# 문서
diff -rq legacy/docs/srs   docs/srs      # SRS.md differ
diff -rq legacy/docs/sdd   docs/sdd      # SDD.md differ
diff -rq legacy/docs/specs docs/specs    # 3 files differ
diff -rq legacy/docs/sa    docs/sa       # 3 files differ (재생성)
# 코드
diff -rq legacy/include    include       # types.hpp, cleaning_engine.hpp, obstacle_handler.hpp
diff -rq legacy/src        src           # main.cpp, cleaning_engine.cpp, obstacle_handler.cpp
diff -q  legacy/CMakeLists.txt CMakeLists.txt   # (출력 없음 = 일치)
# 테스트
diff -rq legacy/test       test          # simulator 1, system 2, unit 2 differ
```

`Only in legacy/...: .gitkeep` 항목은 기준선 폴더 유지를 위한 빈 placeholder로 산출물 차이가 아니다.

---

## 5. 의미 변화 한 줄 요약

회피 경로 선택이 **"좌측 가능→좌회전 / 좌측 막힘→후진"** 으로 단순화되었다(우측 센서 사전 조회 경로 제거). 단, 후진 후 좌측이 여전히 막혀 있을 때의 **blind 최후 탈출 우회전**은 우측 센서 없이 수행되므로 코드(`executeBackAndTurn`, 좌 센서만 사용)·`MotorCommand::TURN_RIGHT`는 **존속(일치)**. 삭제된 것은 우측 **근접 센서(입력)**이지 우회전 **이동 능력(출력)**이 아니다.

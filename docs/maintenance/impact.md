# 변경 영향 분석 (Change Impact) — 오른쪽 근접 센서 삭제

> 입력: `reference/new_objective.md` + `legacy/` (TP#5 완성 산출물).
> 원본 출처(PDF·`CICD-Team7-main/`·`objective.md`)는 열람하지 않았다.
> 이 문서는 `/srs`~`/package`의 작업 스코프 기준이다.

## 1. 변경 요약

- RVC의 근접 센서를 **3개(앞·왼쪽·오른쪽) → 2개(앞·왼쪽)**로 축소한다. **오른쪽 근접 센서를 삭제**한다.
- legacy에서 근접 센서는 단일 인터페이스 `IProximitySensor`를 **3 인스턴스(front/left/right)**로 구분해 사용한다. 따라서 삭제는 "인터페이스 변경"이 아니라 **`right` 인스턴스와 그 배선(wiring)·그 인스턴스에 의존하던 회피 분기**의 삭제다.
- 회피 경로 선택 로직(`ObstacleHandler::selectPath()`)이 "좌측 가능 → 좌회전 / 좌측 막힘 → 후진 후 재탐색"으로 단순화된다 (legacy의 "좌 가능→좌, 좌막힘·우 가능→우, 둘 다 막힘→후진"에서 우측 분기 제거).

## 2. ⚠ 핵심 구분 — 무엇이 삭제 대상이고 무엇이 아닌가

오른쪽 "센서"와 오른쪽 "회전"은 별개다. 혼동하면 과잉 삭제가 된다.

| 토큰 | 정체 | 판정 |
|---|---|---|
| `right` / `right_` `IProximitySensor` 인스턴스 | **오른쪽 근접 센서** | **삭제 (MODIFY)** |
| `MotorCommand::TURN_RIGHT` | **모터 구동 명령** (우회전). 센서가 아니다. | **유지 (KEEP)** — 로봇은 여전히 물리적으로 우회전 가능 |
| `AvoidPath::TURN_RIGHT` | "우측이 비어서 우회전" 회피 **경로 선택지**. 우측 센서 판독에 의존. | **삭제 (MODIFY)** — 우측 센서 없으면 선택 불가 |
| `ObstacleHandler::executeBackAndTurn()` | 후진 후 **좌 센서만** 재확인해 좌/우 회전 결정 | **유지 (KEEP)** — 우측 센서 미사용. 단 §5 의미 검토 대상 |

## 3. 영향 매트릭스 (legacy 기준 경로)

### 3.1 명세 — `docs/specs/` (TP#5 재증류 중간 산출물)

| 파일 | 항목 | 판정 | 근거 |
|---|---|---|---|
| `legacy/docs/specs/system-overview.md` | L18 context diagram `RS["우측 센서"]` | 변경 | 우측 센서 노드 제거 |
| `legacy/docs/specs/requirements.md` | L31 "좌·우측 센서 조회", L35 결정 표 `우측` 열 | 변경 | 우측 열·문구 제거, 결정 표 단순화 |
| `legacy/docs/specs/use-cases.md` | L47 결정 표 `우측` 열, L113 "UC5 Avoid – Turn Right" 매핑 | 변경 | 우측 경로 흡수 항목 제거 |

> ⚠ **워크플로우 주의**: `docs/specs`는 TP#5 `/extract-specs`의 중간 산출물이며, `new_objective.md`의 공식 산출물 목록(SRS·SDD·Code·UT·ST·SA·Simulator)에 **포함되지 않는다**. 전담 커맨드도 없다(`/change-impact`가 대체). → `/srs`가 `legacy/docs/specs`를 참조하므로, **일관성을 위해 `/srs` 단계에서 함께 갱신**하거나 중간 산출물로서 legacy 유지 중 하나를 택한다. (REVIEW: §5)

### 3.2 SRS — `legacy/docs/srs/SRS.md` → **변경**

| 섹션/라인 | 항목 | 판정 |
|---|---|---|
| L68 | context diagram `RS["우측 근접 센서"]` 노드 | 변경 |
| L82 | 외부 액터 표 "우측 근접 센서" 행 | 변경 |
| L168, L175 | 회피 요구사항 + 결정 표 헤더 `전방\|좌측\|우측` | 변경 (우측 열 제거, 표 행 단순화) |
| L409 | UC 주 액터 "전방·좌측·우측 근접 센서" | 변경 → "전방·좌측" |
| L420 | UC 타이밍 결정 표 `우측` 열 | 변경 |
| L496 | 외부 인터페이스 "우측 근접 센서 인터페이스" 행 | 변경 |

### 3.3 SDD — `legacy/docs/sdd/SDD.md` → **변경**

| 섹션/라인 | 항목 | 판정 |
|---|---|---|
| L111 | 매핑 표 "Front/Left/RightSensor → IProximitySensor 3 인스턴스" | 변경 → 2 인스턴스 |
| L142, L164 | `CleaningEngine` ctor `rightSensor` 인자 + `right_` 멤버 | 변경 (제거) |
| L201, L220 | `ObstacleHandler` ctor `right` 인자 + `right_` 멤버 | 변경 (제거) |
| L533, L543 | 회피 시퀀스 다이어그램 `participant RS IProximitySensor(right)`, `execute(TURN_LEFT 또는 TURN_RIGHT)` | 변경 (우측 센서 조회·우회전 분기 제거) |
| L637 | `AvoidPath::TURN_RIGHT // 전방·좌 막힘, 우측 공간 있음` | 변경 (제거) |
| L623 | `MotorCommand::TURN_RIGHT` enum | **일치** (모터 명령, 유지) |
| L839, L846 | Stub 예시 `front, left, right` + handler 조립 예시 | 변경 (예시 코드 갱신) |

### 3.4 Code — `legacy/include`, `legacy/src`

| 파일 | 항목 | 판정 |
|---|---|---|
| `legacy/include/common/types.hpp` | `AvoidPath::TURN_RIGHT` 멤버 | 변경 (제거) |
| `legacy/include/common/types.hpp` | `MotorCommand::TURN_RIGHT` 멤버 | **일치** (모터 명령) |
| `legacy/include/interface/i_proximity_sensor.hpp` | `isBlocked()` 단일 메서드 | **일치** (방향 비특정) |
| `legacy/include/core/cleaning_engine.hpp` | `rightSensor` 인자 + `right_` 멤버 | 변경 (제거) |
| `legacy/src/core/cleaning_engine.cpp` | ctor `rightSensor`, `right_` 초기화, `obstacleHandler_(leftSensor, rightSensor, …)` | 변경 → `obstacleHandler_(leftSensor, …)` |
| `legacy/include/core/obstacle_handler.hpp` | `right` 인자 + `right_` 멤버 + 주석 "using left_ and right_" | 변경 (제거) |
| `legacy/src/core/obstacle_handler.cpp` | `selectPath()` 의 `if(!right_.isBlocked()) return TURN_RIGHT;` (L40) | 변경 (제거) |
| `legacy/src/core/obstacle_handler.cpp` | `avoidAndReturn()` 의 `case AvoidPath::TURN_RIGHT` 블록 (L24–26) | 변경 (제거) |
| `legacy/src/core/obstacle_handler.cpp` | `executeBackAndTurn()` (L51–60, 좌 센서만 사용) | **일치** (단 §5 검토) |
| `legacy/src/app/main.cpp` | `DemoSensor front, left, right;` (L54), `engine(front, left, right, …)` (L60) | 변경 (우측 인스턴스·전달 제거) |
| `legacy/src/app/main.cpp` | `DemoDriveTrain` names 배열의 `"TURN_RIGHT"` 문자열 (L29) | **일치** (모터 명령 라벨) |
| 그 외 `clean_intensity_controller.*`, `console_log.*`, `cli_command_source.*`, `i_dust_sensor/i_drive_train/i_cleaning_unit/i_output_log/i_command_source.hpp` | — | **일치** (센서 무관) |
| `legacy/CMakeLists.txt` | 소스 glob, 센서별 항목 없음 | **일치** |

### 3.5 UT — `legacy/test/unit/`

| 파일 | 항목 | 판정 |
|---|---|---|
| `test/unit/test_obstacle_handler.cpp` | 픽스처 `front, left, right` + `handler{left, right, …}` (L9,12) | 변경 (우측 stub·인자 제거) |
| 〃 | `TurnRightWhenLeftBlockedRightClear` (L33–40) | 변경 (삭제 — 우측 센서 경로) |
| 〃 | `selectPath` 호출 카운트 검증 (L61,77–85: right.callCount==0 등) | 변경 (우측 polling 검증 제거) |
| 〃 | BackPath 후진→`TURN_RIGHT` 기대 (L103–118) | 변경 + **검토** (executeBackAndTurn 의미 결정에 종속, §5) |
| `test/unit/test_cleaning_engine.cpp` | 픽스처 `front,left,right` + `CleaningEngine(front,left,right,…)` (L12,20,62) | 변경 (우측 인자 제거) |
| `test/unit/test_clean_intensity_controller.cpp` | — | **일치** (센서 무관) |
| `test/unit/stub/stub_proximity_sensor.hpp` | 방향 비특정 클래스 | **일치** (인스턴스 수만 테스트에서 변동) |
| 그 외 stub(`dust/drive/cleaning_unit/output/command`) | — | **일치** |

### 3.6 Simulator — `legacy/test/simulator/`

| 파일 | 항목 | 판정 |
|---|---|---|
| `test/simulator/rvc_sim_harness.hpp` | `right_` 멤버(L100), `engine_(…, right_, …)`(L24), `setRightBlocked`(L33), `injectRightObstacle`(L41), `rightSensorCalls`(L79), `reset` 내 `right_`(L85), `rightSensor()`(L92) | 변경 (우측 주입·조회 API 및 배선 제거) |
| `test/simulator/sim_proximity_sensor.hpp` | 방향 비특정 클래스 | **일치** |
| 그 외 sim(`dust/drive/cleaning_unit/output/command`) | — | **일치** |

### 3.7 ST — `legacy/test/system/`

| 파일 | 항목 | 판정 |
|---|---|---|
| `test/system/test_system_scenarios.cpp` | 픽스처 `front,left,right`(L14) + `engine{front,left,right,…}`(L21) | 변경 |
| 〃 | `ObstacleAvoidanceRightPath` (L52–62), `f.right.setBlocked`(L57,71) | 변경 (삭제 — 우측 경로 시나리오) |
| 〃 | 오류처리 시나리오 `(broken, left, right, …)`(L138,147) | 변경 (우측 제거) |
| `test/system/test_uc_scenarios.cpp` | `UC03 … RightPath_WhenLeftBlockedRightClear` (L94–107), `setRightBlocked`(L100,117) | 변경 (삭제) |
| 〃 | 오류처리 시나리오 `(faulty, left, right, …)`(L341,350,368,377) | 변경 (우측 제거) |

### 3.8 SA — `legacy/docs/sa/`

| 파일 | 판정 | 근거 |
|---|---|---|
| `docs/sa/cppcheck.txt`, `clang-tidy.txt`, `summary.md` | 변경 (재생성) | `/sa`가 수정 코드 기준 재실행, legacy 리포트 대비 이슈 증감 비교 |

## 4. 단계별 스코프 (후속 커맨드 작업 지시서)

- **`/srs`**: `legacy/docs/srs/SRS.md` 인계 → L68·82·168·175·409·420·496 수정. (선택) `docs/specs` 3파일 일관 갱신 — §3.1 주의 참고.
- **`/sdd`**: `legacy/docs/sdd/SDD.md` 인계 → L111·142·164·201·220·533·543·637·839·846 수정. `MotorCommand::TURN_RIGHT`(L623)는 일치 유지. `AvoidPath::TURN_RIGHT` 제거 확정(§5).
- **`/code`**: `types.hpp`(AvoidPath만), `cleaning_engine.{hpp,cpp}`, `obstacle_handler.{hpp,cpp}`, `main.cpp` 수정. `i_proximity_sensor.hpp`·`MotorCommand`·`executeBackAndTurn`·`CMakeLists.txt`는 일치. WSL 클린 빌드.
- **`/ut`**: `test_obstacle_handler.cpp`, `test_cleaning_engine.cpp` 수정. `stub_proximity_sensor.hpp`·`test_clean_intensity_controller.cpp` 일치.
- **`/simulator`**: `rvc_sim_harness.hpp` 수정. `sim_proximity_sensor.hpp` 일치.
- **`/st`**: `test_system_scenarios.cpp`, `test_uc_scenarios.cpp` 수정 (우측 경로 시나리오 삭제, 오류처리 시나리오 인자 정리).
- **`/sa`**: 수정 코드 재분석 + legacy 리포트 대비 비교.
- **`/package`**: 통합 diff 매트릭스 + 클린 빌드 재현 + README/INDEX.

## 5. 의미 변화 노트 & 검토(REVIEW) 항목

1. **회피 경로 단순화 (확정)**: `selectPath()`는 "좌 가능→`TURN_LEFT`, 아니면 `BACK_AND_TURN`"으로 축소. `avoidAndReturn()` 스위치의 `TURN_RIGHT` 케이스 제거.
2. **`AvoidPath::TURN_RIGHT` 처리 (REVIEW → /sdd·/code)**: `selectPath`에서 더 이상 반환되지 않아 dead가 된다. **권장: enum 멤버와 스위치 케이스를 함께 제거**(최소·정합). 단순 미사용으로 남기는 것은 비권장.
3. **`executeBackAndTurn()` 의 사후-백업 회전 (REVIEW → /srs·/sdd)**: legacy는 후진 후 **좌 센서만** 재확인해 좌 막힘이면 `TURN_RIGHT`(blind). 우측 센서 미사용이라 **코드상 변경은 강제되지 않음(일치)**. 그러나 "우측 센서 없는 방향으로 blind 우회전"이 새 요구(센서 2개)에서 의미상 타당한지 `/srs`·`/sdd`가 확정해야 한다. 결정에 따라 §3.5의 BackPath 단위 테스트 기대값이 종속적으로 바뀐다.
4. **`docs/specs` 소유권 (REVIEW)**: §3.1 주의 — 전담 커맨드 없음. `/srs`에서 함께 갱신 권장.

## 6. 집계

- **변경(MODIFY)**: 문서 5종(specs 3 + SRS + SDD) · 코드 5파일(types 일부, engine h/cpp, handler h/cpp, main) · UT 2파일 · Simulator 1파일 · ST 2파일 · SA 3파일 = **약 18개 파일/아티팩트**
- **일치(KEEP)**: `i_proximity_sensor.hpp`, `stub_proximity_sensor.hpp`, `sim_proximity_sensor.hpp`, `MotorCommand::TURN_RIGHT`, `executeBackAndTurn()`, `CMakeLists.txt`, 비센서 컴포넌트(clean_intensity_controller·console_log·cli_command_source·dust/drive/cleaning/output/command 계열) 및 그 stub/sim, `test_clean_intensity_controller.cpp` 등 **다수**
- **검토(REVIEW)**: 4건 (§5)

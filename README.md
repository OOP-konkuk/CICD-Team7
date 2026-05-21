# RVC Control SW — 조교 재현 가이드

본 디렉토리는 객체지향개발방법론 **TeamPractice#5**의 산출물입니다.
RVC(Robot Vacuum Cleaner) Control SW를 **Claude Code(AI)** 를 일관되게 활용하여 처음부터 개발했습니다.

> **재증류·재설계 정책**: `reference/`의 PDF, `objective.md`, 이전 학기 코드(`CICD-Team7-main/`)는 시스템의 의도를 *이해*하기 위한 1차 자료로만 사용했습니다. **요구사항(FR/NFR/UC) 자체도 Claude가 통독·이해한 후 새 ID 체계로 재증류**했으며, 원본의 문장·다이어그램·시그니처는 산출물에 그대로 옮기지 않았습니다. 추적성 표로 원본 ID와 새 ID 매핑을 입증합니다. 아키텍처·클래스 분해·인터페이스는 객체지향 설계 원칙에 따라 처음부터 도출했습니다.

> 팀원이 AI 자동화 환경을 사용하는 방법은 [USAGE.md](USAGE.md)를 참고하세요.

---

## 1. 산출물 위치

| 산출물 | 위치 | 생성 단계 |
|---|---|---|
| 시스템 개요 (Claude 재증류) | `docs/specs/system-overview.md` | `/extract-specs` |
| 재증류된 요구사항 (FR/NFR + 추적성 표) | `docs/specs/requirements.md` | `/extract-specs` |
| 재증류된 유스케이스 | `docs/specs/use-cases.md` | `/extract-specs` |
| SRS (Claude 재구성) | `docs/srs/SRS.md` | `/srs` |
| SDD (Claude 최적화 설계) | `docs/sdd/SDD.md` | `/sdd` |
| C++ 소스 (Claude 구현) | `include/`, `src/` | `/code` |
| 단위 테스트 | `test/unit/*.cpp` | `/ut` |
| 시뮬레이터 | `test/simulator/*.hpp` | `/simulator` |
| 시스템 테스트 | `test/system/*.cpp` | `/st` |
| 정적 분석 리포트 | `docs/sa/{cppcheck.txt,clang-tidy.txt,summary.md}` | `/sa` |
| 빌드 설정 | `CMakeLists.txt` | `/code` |
| Claude 작동 기록 (발표용) | `log/YYYY-MM-DD_HHMM_<command>.md` | 매 슬래시 커맨드 |

---

## 2. 환경 요구사항

- OS: **Ubuntu 22.04** (또는 Windows + WSL Ubuntu)
- 필수 패키지:
  ```bash
  sudo apt update && sudo apt install -y build-essential cmake
  ```
- 권장 패키지 (정적 분석):
  ```bash
  sudo apt install -y cppcheck clang-tidy lcov
  ```

> **Note**: GoogleTest는 `CMakeLists.txt`의 `FetchContent`로 자동 다운로드됩니다. 별도 설치 불필요.

---

## 3. 빌드·테스트 (조교용)

### 경로 제약 사항

프로젝트 경로에 `#` 문자가 포함되어 있어 GNU Make의 주석 처리와 충돌합니다.
**심볼릭 링크**를 통해 우회합니다 (일회성 설정):

```bash
ln -sfn "$(pwd)" /tmp/rvc_proj
```

### 한 줄 빌드·테스트

```bash
cmake -S /tmp/rvc_proj -B /tmp/rvc_build && \
cmake --build /tmp/rvc_build -j && \
ctest --test-dir /tmp/rvc_build --output-on-failure
```

### Windows/WSL에서 실행

```powershell
wsl bash -lc "ln -sfn '/mnt/c/Users/윤성진/OneDrive/바탕 화면/Konkuk/3-1/객체지향개발방법론/과제/TeamPractice#5/dev/#5_Team7' /tmp/rvc_proj && cmake -S /tmp/rvc_proj -B /tmp/rvc_build && cmake --build /tmp/rvc_build -j && ctest --test-dir /tmp/rvc_build --output-on-failure"
```

### 빌드 산출물

| 파일 | 설명 |
|------|------|
| `/tmp/rvc_build/oop` | 메인 실행파일 |
| `/tmp/rvc_build/oop_test` | 단위 테스트 |
| `/tmp/rvc_build/oop_system_test` | 시스템 테스트 |

---

## 4. 빌드·테스트 결과 요약 (2026-05-21 패키징 기준)

| 항목 | 결과 |
|------|------|
| 빌드 경고 | **0개** |
| 빌드 오류 | **0개** |
| 단위 테스트 (test/unit/) | **37/37 통과** |
| 시스템 테스트 (test/system/) | **19/19 통과 (UC-01~UC-06)** |
| 기존 통합 테스트 (test/system/) | **7/7 통과** |
| 전체 | **56/56 (100%)** |
| 총 테스트 시간 | 약 0.31초 |
| 정적 분석 (GCC -Wall -Wextra …) | **0 warnings, 0 errors** |

---

## 5. 정적 분석 재실행

```bash
# GCC 컴파일러 진단 (cppcheck 대체)
g++ -std=c++17 -Wall -Wextra -Wpedantic -Wconversion -Wshadow \
    -I/tmp/rvc_proj/include -c \
    /tmp/rvc_proj/src/core/cleaning_engine.cpp \
    /tmp/rvc_proj/src/core/obstacle_handler.cpp \
    /tmp/rvc_proj/src/core/clean_intensity_controller.cpp

# 설치된 경우 cppcheck
cppcheck --enable=all --std=c++17 --suppress=missingIncludeSystem \
    --suppress=unusedFunction -I include src 2> docs/sa/cppcheck.txt

# 설치된 경우 clang-tidy
find src include -name '*.cpp' | \
    xargs clang-tidy -p /tmp/rvc_build > docs/sa/clang-tidy.txt 2>&1 || true
```

분석 요약: `docs/sa/summary.md`

---

## 6. AI 자동화 워크플로우 (재현)

Claude Code에서 다음 슬래시 커맨드를 순서대로 호출하면 동일한 산출물이 재생성됩니다:

```
/extract-specs → /srs → /sdd → /code → /ut → /simulator → /st → /sa → /package
```

각 커맨드는 `log/` 폴더에 실행 기록을 자동으로 남깁니다.
의사결정 흐름 전체는 **[log/INDEX.md](log/INDEX.md)** 참조.

---

## 7. AI 작동 기록 인덱스

| 커맨드 | 로그 파일 | 핵심 결정 |
|--------|-----------|-----------|
| `/extract-specs` | [2026-05-21_1414_extract-specs.md](log/2026-05-21_1414_extract-specs.md) | PDF 9 UC → 6 UC 재증류; 시간 한계를 FR 검증 기준에 내재화 |
| `/srs` | [2026-05-21_1422_srs.md](log/2026-05-21_1422_srs.md) | 재증류 ID(FR-CTRL/MOVE/CLEAN/SENSE, NFR-TIMING/ARCH/SAFETY) 확정 |
| `/sdd` | [2026-05-21_1439_sdd.md](log/2026-05-21_1439_sdd.md) | 4계층 아키텍처; CleaningEngine·ObstacleHandler·CleanIntensityController 3 컴포넌트 |
| `/code` | [2026-05-21_1458_code.md](log/2026-05-21_1458_code.md) | resume() 추가; FetchContent 채택; executeTurn() 원자 패턴 |
| `/ut` | [2026-05-21_1513_ut.md](log/2026-05-21_1513_ut.md) | 37 테스트; 경계값(5000ms), 누적 틱, 부스트 중 장애물 추가 |
| `/simulator` | [2026-05-21_1542_simulator.md](log/2026-05-21_1542_simulator.md) | RVCSimHarness 하니스; 이중 모드 주입(setBlocked+enqueueEvent) |
| `/st` | [2026-05-21_1558_st.md](log/2026-05-21_1558_st.md) | UC-01~06 100% 커버; RVCSimHarness 활용 19 시나리오 |
| `/sa` | [2026-05-21_1554_sa.md](log/2026-05-21_1554_sa.md) | front_ 미사용 멤버 제거; C-배열→std::array; GCC -fanalyzer 거짓 양성 억제 |
| `/package` | [2026-05-21_1629_package.md](log/2026-05-21_1629_package.md) | 클린 빌드 56/56; # 경로 symlink 해결책 CLAUDE.md 등록 |

---

## 8. 디렉토리 트리

```
dev/#5_Team7/
├── CLAUDE.md                   # AI 운영 규약
├── README.md                   # 본 파일
├── USAGE.md                    # 팀원용 가이드
├── CMakeLists.txt              # 빌드 설정 (FetchContent GTest)
├── .claude/commands/           # 9개 슬래시 커맨드 정의
├── docs/
│   ├── specs/                  # PDF 재증류 명세 (/extract-specs)
│   ├── srs/SRS.md              # 소프트웨어 요구사항 명세 (/srs)
│   ├── sdd/SDD.md              # 소프트웨어 설계 문서 (/sdd)
│   └── sa/                     # 정적 분석 리포트 (/sa)
├── include/
│   ├── common/types.hpp        # 공용 enum 타입
│   ├── interface/              # 6개 순수 추상 인터페이스
│   ├── core/                   # CleaningEngine, ObstacleHandler, CleanIntensityController
│   └── app/                    # ConsoleLog, CliCommandSource
├── src/
│   ├── core/                   # 3개 컴포넌트 구현
│   └── app/                    # 2개 어댑터 + main.cpp
├── test/
│   ├── unit/                   # GTest 단위 테스트 (37개)
│   │   └── stub/               # 6개 Stub 헤더
│   ├── simulator/              # 6개 Sim 구현 + RVCSimHarness
│   └── system/                 # GTest 시스템 테스트 (26개)
├── log/                        # Claude 작동 기록 (발표용)
│   └── INDEX.md                # 의사결정 인덱스
└── reference/                  # 원본 명세 (수정 금지)
```

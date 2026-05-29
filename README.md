# RVC Control SW — 조교 재현 가이드 (TeamPractice#6 유지보수)

본 디렉토리는 객체지향개발방법론 **TeamPractice#6**의 산출물입니다.
이전 과제(TP#5)에서 완성한 RVC(Robot Vacuum Cleaner) Control SW를 **유지보수**하여, 요구사항 변경(**오른쪽 근접 센서 삭제, 센서 3개 → 2개**)을 모든 아티팩트에 반영했습니다. 작업은 **Claude Code(AI)** 로 수행했습니다.

> **유지보수 정책**: 바닥부터 다시 만들지 않았습니다. TP#5 완성 산출물을 `legacy/`에 **읽기전용 기준선**으로 두고, 오른쪽 센서 삭제가 강제하는 부분만 최소 수정했습니다. **수정한 아티팩트는 `legacy/` 대비 변경점(diff)을, 변경하지 않은 항목은 "일치"를 명시**합니다. TP#5가 참고했던 원본(PDF, `CICD-Team7-main/`, `objective.md`)은 본 과제에서 **다시 열지 않았습니다** — 유지보수의 진실은 `legacy/`입니다.

> 팀원이 AI 자동화 환경을 사용하는 방법은 [USAGE.md](USAGE.md)를 참고하세요.

## 1. 산출물 위치 (수정본 = 최상위, 기준선 = `legacy/`)

| 산출물 | 수정본 위치 | 기준선(legacy) | 생성 단계 |
|---|---|---|---|
| 변경 영향 분석 | `docs/maintenance/impact.md` | — (신규) | `/change-impact` |
| 통합 diff 매트릭스 | `docs/maintenance/diff-matrix.md` | — (신규) | `/package` |
| SRS | `docs/srs/SRS.md` | `legacy/docs/srs/SRS.md` | `/srs` |
| SDD | `docs/sdd/SDD.md` | `legacy/docs/sdd/SDD.md` | `/sdd` |
| Code | `include/`, `src/`, `CMakeLists.txt` | `legacy/{include,src,CMakeLists.txt}` | `/code` |
| UT | `test/unit/*` | `legacy/test/unit/*` | `/ut` |
| Simulator | `test/simulator/*` | `legacy/test/simulator/*` | `/simulator` |
| ST | `test/system/*` | `legacy/test/system/*` | `/st` |
| SA | `docs/sa/{cppcheck.txt,clang-tidy.txt,summary.md}` | `legacy/docs/sa/*` | `/sa` |
| Claude 작동 기록 (발표용) | `log/YYYY-MM-DD_HHMM_<command>.md` | `legacy/log/*` | 매 슬래시 커맨드 |

## 2. 환경 요구사항 (legacy와 동일)

- OS: **Ubuntu 22.04** (또는 Windows + WSL Ubuntu)
- 패키지:
  ```bash
  sudo apt update && sudo apt install -y \
      build-essential cmake ninja-build libgtest-dev cppcheck clang-tidy lcov
  ```

## 3. 한 줄 빌드·테스트 (조교용)

### Linux/WSL
```bash
cmake -S . -B build && cmake --build build -j && ctest --test-dir build --output-on-failure
```

### Windows PowerShell (WSL 경유)
```powershell
wsl bash -lc "cd '/mnt/c/Users/윤성진/OneDrive/바탕 화면/Konkuk/3-1/객체지향개발방법론/과제/TeamPractice#6/#5_Team7_NoRight' && cmake -S . -B build && cmake --build build -j && ctest --test-dir build --output-on-failure"
```

빌드 산출물: `build/oop` (메인), `build/oop_test` (단위 테스트), `build/oop_system_test` (시스템 테스트).

#### 최종 빌드·테스트 결과 (2026-05-29 클린 빌드 재현)

| 항목 | 결과 |
|---|---|
| 클린 빌드 (`rm -rf build` 후) | **성공, 경고 0** |
| 전체 테스트 (`ctest`) | **53/53 통과 (100%)** |
| └ 단위 테스트 (`oop_test`) | 29건 통과 |
| └ 시스템 테스트 (`oop_system_test`) | 24건 통과 |
| 정적 분석 (`docs/sa/`) | error/warning/style/performance/portability 전부 0 (legacy 일치) |

> 시스템 테스트는 GTest `gtest_discover_tests`로 개별 테스트명(`UC0*`, `SystemScenario.*`)으로 등록됩니다. 시스템 테스트만 돌리려면 `ctest --test-dir build -R 'UC0|SystemScenario'` 또는 `./build/oop_system_test` 를 사용하세요.

## 4. legacy 대비 변경 확인 (유지보수 검증의 핵심)

수정본과 기준선을 한 폴더에서 직접 비교할 수 있습니다:

```bash
for d in docs include src test CMakeLists.txt; do echo "=== $d ==="; diff -rq legacy/$d $d 2>/dev/null; done
```

변경/일치 전체 요약은 `docs/maintenance/diff-matrix.md`에 통합되어 있습니다.

## 5. AI 자동화 워크플로우 (유지보수 재현)

Claude Code에서 다음 슬래시 커맨드를 순서대로 호출하면 동일한 유지보수 결과가 재생성됩니다:

```
/change-impact   # 오른쪽 센서 삭제의 영향 범위 분석
/srs             # SRS 인계 후 수정
/sdd             # SDD 인계 후 수정
/code            # 구현 인계 후 수정 + 빌드
/ut              # 단위 테스트 수정 + 실행
/simulator       # 시뮬레이터 수정
/st              # 시스템 테스트 수정 + 실행
/sa              # 정적 분석 재실행
/package         # 재현성 점검 + 통합 diff 매트릭스 + README 최종화
```

각 커맨드는 `log/`에 실행 기록을 자동으로 남기며, 특히 **`## 차이 표기`** 섹션에 legacy 대비 변경/일치를 기록합니다 — 이것이 본 과제 발표 자료의 핵심입니다.

전 단계 작동 기록과 각 단계의 변경/일치 1줄 요약은 **[log/INDEX.md](log/INDEX.md)** 에 인덱싱되어 있습니다.

자세한 커맨드 정의는 `.claude/commands/<command>.md`를 참고하세요.

## 6. 디렉토리 트리

```
#5_Team7_NoRight/
├── CLAUDE.md                 # AI 운영 규약 (유지보수 모드)
├── README.md                 # 본 파일
├── USAGE.md                  # 팀원용 가이드
├── CMakeLists.txt
├── .claude/commands/         # 9개 슬래시 커맨드 (유지보수 모드)
├── legacy/                   # ★ TP#5 완성 산출물 (읽기전용 기준선, 수정 금지)
├── docs/{maintenance,srs,sdd,sa,specs}/
├── include/, src/            # 수정된 C++ 소스
├── test/{unit,system,simulator}/
├── log/                      # Claude 작동 기록 (발표용)
└── reference/                # new_objective.md (+ 열람 금지 원본: PDF, CICD-Team7-main, objective.md)
```

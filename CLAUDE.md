# RVC Control SW — Claude Code 운영 규약 (TP#6 메인터넌스)

이 파일은 모든 Claude Code 세션에서 자동으로 로드된다. 새 대화를 시작할 때 가장 먼저 이 규약을 따른다.

## 1. 프로젝트

- **이름**: RVC (Robot Vacuum Cleaner) Control SW — **유지보수(Maintenance) 단계**
- **기준선(legacy) 출처**: 이전 과제(TeamPractice#5)에서 완성한 산출물 일습. 본 작업 폴더의 `legacy/`에 읽기전용으로 복사되어 있다.
- **변경 명세 출처**: `reference/new_objective.md`
- **본 과제(TeamPractice#6)의 목표**:
  - **요구사항 변경**: RVC의 근접 센서를 3개(앞/왼쪽/오른쪽)에서 **2개(앞/왼쪽)로 축소** — **오른쪽 센서 삭제**.
  - 이 변경에 영향받는 **모든 아티팩트(SRS, SDD, Code, UT, ST, SA, Simulator)를 유지보수**한다.
  - **바닥부터 다시 만들지 않는다.** `legacy/`를 기준선으로, 영향받는 부분만 최소 수정한다.
  - 수정한 아티팩트가 `legacy/` 대비 **다르면 그 차이를, 같으면 "일치"를 명시**한다.
  - 조교가 이 폴더 하나로 **재현 가능**해야 한다. Claude의 **작동 기록 자체가 발표 자료**이며, 모든 슬래시 커맨드는 `log/`에 기록을 남긴다.

## 2. 핵심 원칙 — 최소 수정 메인터넌스 (Greenfield 아님)

본 작업환경의 가장 중요한 원칙이다. (TP#5의 "재증류·재설계" 원칙은 **폐기**되었다.)

- **기준선은 `legacy/`다.** `legacy/`는 TP#5의 완성 산출물이며, 본 과제의 모든 수정은 이 기준선에서 출발한다.
- **재사용 가능한 구조는 그대로 둔다.** 클래스 구조·파일 분할·이름·계층·시퀀스를 임의로 재설계하지 않는다. 오른쪽 센서 삭제가 강제하지 않는 변경은 하지 않는다.
- **영향받는 아티팩트만 수정한다.** 무엇이 영향받는지는 `/change-impact`가 먼저 산출한 `docs/maintenance/impact.md`가 기준이다.
- **차이 명시 의무**: 모든 수정 아티팩트는 `legacy/`의 대응 파일 대비 **변경점(diff)** 또는 **"일치(legacy와 동일)"**를 명시해야 한다. 이것이 본 과제의 채점 핵심이다.
- **오른쪽 센서 삭제의 본질**: 앞 센서는 정지/회피 트리거, 좌/우 센서는 회피 경로 선택(좌측 우선 → 우측 → 후진)에 쓰였다. 우측 센서를 제거하면 회피 경로 선택은 "좌측 가능하면 좌회전, 아니면 후진"으로 단순화된다. 이 의미적 변화를 모든 계층(요구사항→설계→코드→테스트)에서 일관되게 반영한다.

> 요약: "`legacy/`가 진실. 오른쪽 센서 삭제가 닿는 곳만 최소로 고치고, 그 차이를 명시한다. 재설계 금지."

## 3. Tech Stack (legacy 그대로 유지)

- 언어: **C++17**
- 빌드: **CMake ≥ 3.14**
- 테스트: **GoogleTest (GTest)**
- 컴파일러: **GCC/G++** (Ubuntu)
- 정적 분석: **cppcheck**, **clang-tidy**
- 커버리지: **lcov**

스택·도구·버전은 `legacy/`와 동일하게 유지한다. 새 의존성을 도입하지 않는다.

## 4. 빌드 환경

**WSL Ubuntu 전용**. Windows 네이티브 빌드는 사용하지 않는다.

Windows에서 명령을 실행할 때는 항상 `wsl` 접두사를 사용한다:

```powershell
wsl bash -lc "cd '/mnt/c/Users/.../[#5_Team7_NoRight]' && cmake -S . -B build && cmake --build build -j && ctest --test-dir build --output-on-failure"
```

WSL 진입 시 작업 디렉토리 경로:
```
/mnt/c/Users/윤성진/OneDrive/바탕 화면/Konkuk/3-1/객체지향개발방법론/과제/TeamPractice#6/#5_Team7_NoRight
```

필요 패키지(조교 환경 가정, legacy와 동일):
```bash
sudo apt update && sudo apt install -y build-essential cmake ninja-build libgtest-dev cppcheck clang-tidy lcov
```

## 5. 디렉토리 규약

```
#5_Team7_NoRight/
├── CLAUDE.md                # 본 파일
├── README.md                # 조교 재현 가이드 (유지보수 워크플로우)
├── USAGE.md                 # 팀원용 사용 가이드
├── .claude/commands/        # 9개 슬래시 커맨드 정의 (메인터넌스 모드)
├── legacy/                  # ★ TP#5 완성 산출물 (읽기전용 기준선) — 수정 금지
│   ├── docs/ include/ src/ test/ log/ CMakeLists.txt README.md USAGE.md
├── docs/
│   ├── maintenance/         # /change-impact 결과 (impact.md) + 통합 diff 매트릭스
│   ├── specs/               # 수정된 명세 (legacy/docs/specs 인계 후 수정)
│   ├── srs/                 # 수정된 SRS
│   ├── sdd/                 # 수정된 SDD
│   └── sa/                  # 수정 코드 정적분석 리포트
├── include/                 # 수정된 C++ 헤더
├── src/                     # 수정된 C++ 구현
├── test/
│   ├── unit/                # 수정된 단위 테스트
│   ├── system/              # 수정된 시스템 테스트
│   └── simulator/           # 수정된 시뮬레이터
├── log/                     # 슬래시 커맨드 실행 기록 (발표 자료)
├── reference/               # new_objective.md (+ 열람 금지 원본: PDF, CICD-Team7-main)
└── CMakeLists.txt           # legacy 인계 후 (필요시) 수정
```

수정본은 `legacy/`가 아니라 위 최상위 `docs/`·`include/`·`src/`·`test/`에 둔다. `legacy/`와 수정본을 한 폴더에서 직접 비교할 수 있어야 한다.

## 6. 입력·참조 정책 (가장 중요)

### 6.1 허용되는 입력 (이것만 본다)

- **`legacy/`** — TP#5 완성 산출물. 모든 유지보수의 기준선이자 인계 대상.
- **`reference/new_objective.md`** — 이번 변경 명세(오른쪽 센서 삭제).
- **`docs/maintenance/impact.md`** — `/change-impact`가 산출한 변경영향 범위.
- 직전 단계의 수정 산출물(예: `/code`는 `docs/sdd/SDD.md`의 수정본을 본다).

### 6.2 절대 열람 금지 (원본 출처)

`new_objective.md`의 지시: **"기존 프로젝트가 구현을 위해 참고했던 다른 원본 파일은 참고하지 않는다."** 따라서 다음은 본 과제 어느 단계에서도 **열지 않는다**:

- `reference/객체지향개발방법론#1~#4.pdf` (및 그 외 PDF)
- `reference/CICD-Team7-main/` (이전 학기 코드)
- `reference/objective.md` (TP#5 목표 — 본 과제 입력 아님)

유지보수의 진실은 `legacy/`이지 그 원본이 아니다. 원본을 다시 열면 본 과제의 전제(레거시 기반 최소 수정)가 깨진다.

### 6.3 보호

- `reference/` 폴더의 모든 파일은 **읽기 전용**(그나마 열람 가능한 것은 `new_objective.md`뿐). 수정·이동·삭제 금지.
- **`legacy/` 폴더 전체는 읽기 전용 기준선이다. 절대 수정·이동·삭제하지 않는다.** diff의 기준이 훼손되면 본 과제가 성립하지 않는다.

## 7. 로그 정책 (발표 자료)

**모든 슬래시 커맨드는 첫 동작으로 로그 파일을 만든다.**

- 경로: `log/YYYY-MM-DD_HHMM_<command>.md` (WSL `date +%Y-%m-%d_%H%M` 사용)
- 섹션 (고정):
  1. `# /<command>`
  2. `## 사용 프롬프트` — 사용자가 호출한 슬래시 커맨드 한 줄
  3. `## 주요 결정` — 이 단계의 판단 (왜를 적는다)
  4. `## 생성·수정 파일` — 절대 경로 리스트
  5. `## 차이 표기` — **legacy 대비 변경/일치.** 어떤 파일·항목이 바뀌었고(변경 요지), 어떤 것이 legacy와 동일한지(일치)를 표로 남긴다. **이 섹션은 본 과제에서 가장 중요하다.**
  6. `## 결과 요약` — 산출물의 본질 (3~6줄)
  7. `## 검증 방법` — 빌드/테스트 명령 (조교가 그대로 실행 가능해야 함)
  8. `## 다음 단계` — 권장 후속 슬래시 커맨드

이 기록 자체가 발표 슬라이드로 옮겨진다. 특히 "## 차이 표기"에는 **오른쪽 센서 삭제로 인해 무엇이 바뀌고 무엇이 그대로인지**를 근거와 함께 남긴다.

## 8. 슬래시 커맨드 (메인터넌스 단위)

권장 실행 순서:

1. `/change-impact` — `new_objective.md` + `legacy/`를 읽어 오른쪽 센서 삭제가 닿는 아티팩트·파일·ID 목록과 **닿지 않는(일치 예정) 목록**을 `docs/maintenance/impact.md`로 산출
2. `/srs` — `legacy/docs/srs/SRS.md` 인계 → 센서 2개로 수정 → 변경/일치 표기
3. `/sdd` — `legacy/docs/sdd/SDD.md` 인계 → 수정 → 변경/일치 표기
4. `/code` — `legacy/{include,src,CMakeLists.txt}` 인계 → 최소 수정 → WSL 빌드 검증
5. `/ut` — `legacy/test/unit` 인계 → 수정 → ctest
6. `/simulator` — `legacy/test/simulator` 인계 → 수정
7. `/st` — `legacy/test/system` 인계 → 수정 → 실행
8. `/sa` — 수정 코드에 cppcheck + clang-tidy 재실행, legacy 리포트 대비 비교
9. `/package` — 클린 빌드 재현 + 전 아티팩트 통합 diff 매트릭스 + README 최종화

상세 정의는 `.claude/commands/<command>.md` 참조.

## 9. 절대 규칙

- 위 디렉토리/파일 규약을 임의로 바꾸지 않는다.
- **`legacy/` 폴더는 절대 수정·이동·삭제하지 않는다** (diff 기준선).
- **원본 출처(PDF, `CICD-Team7-main/`, `objective.md`)를 어느 단계에서도 열지 않는다.** 입력은 `legacy/` + `new_objective.md`뿐.
- **바닥부터 재설계·재구현하지 않는다.** 오른쪽 센서 삭제가 강제하는 최소 변경만 한다. 재사용 가능한 구조는 그대로 둔다.
- **모든 수정 아티팩트는 legacy 대비 변경/일치를 명시한다** (로그의 `## 차이 표기` + 각 문서 내 변경 이력).
- 슬래시 커맨드 호출 없이 산출물을 만들지 않는다 (로그 기록 누락 방지).
- 새 외부 의존성을 도입하지 않는다 (legacy 스택 유지). 불가피하면 README의 apt 목록과 CMakeLists.txt에 반영.

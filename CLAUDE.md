# RVC Control SW — Claude Code 운영 규약

이 파일은 모든 Claude Code 세션에서 자동으로 로드된다. 새 대화를 시작할 때 가장 먼저 이 규약을 따른다.

## 1. 프로젝트

- **이름**: RVC (Robot Vacuum Cleaner) Control SW
- **명세 출처**: 건국대 객체지향개발방법론 팀프로젝트 #1~#4 (`reference/객체지향개발방법론#1~#4.pdf`, `reference/objective.md`)
- **본 과제(TeamPractice#5)의 목표** (`reference/objective.md`):
  - RVC Control SW의 전체 산출물(**SRS, SDD, Code, UT, ST, SA, Simulator**)을 AI(Claude Code)를 "일관되게" 활용하여 생성한다.
  - 조교가 이 폴더 하나로 **재현 가능**해야 한다.
  - Claude의 **작동 기록 자체가 발표 자료**이다. 모든 슬래시 커맨드 실행은 `log/`에 기록을 남긴다.

## 2. 핵심 원칙 — Reference는 "이해의 입력", 산출물은 Claude가 재증류·재설계

본 작업환경의 가장 중요한 원칙이다.

- `reference/` 폴더(PDF 4개, `objective.md`, `CICD-Team7-main/` 이전 학기 코드)는 **이 시스템의 의도를 이해하기 위한 1차 자료**이지, 산출물에 그대로 옮길 텍스트 원본이 아니다.
- **요구사항 자체(FR/NFR/UC)도 Claude가 재증류한다.** PDF의 R1.1~R6.1, NFR-P/O/OE/I, UC1~UC9를 **그대로 옮겨 쓰지 않는다.** 다음을 수행해야 한다:
  - PDF·코드·objective.md를 모두 읽어 **시스템의 의도(목적·환경·외부 자극·기대 동작·품질 속성)**를 파악
  - 중복·과세분화된 요구사항은 통합, 묵시적 가정은 명시화, 모호한 조건은 검증 가능한 형태로 재진술
  - 새로운 ID 체계(예: FR-NN, NFR-CAT-NN, UC-NN)와 새로운 그룹화로 **더 명료하고 검증 가능한 사양**으로 재구성
  - 단, **원본의 기능 커버리지와 NFR 강제 조항(예: NFR-OE-01 C++17/CMake3.14)은 빠짐없이 보존**해야 한다 — 변형은 형식 최적화일 뿐, 범위 축소가 아니다
- PDF에 포함된 **Domain Model 다이어그램, Use Case Sequence Diagram, System Operations 박스의 메서드 시그니처는 참조하지 않는다.** `CICD-Team7-main/`의 소스 코드 구조·클래스명·메서드명도 마찬가지다. 이들은 팀(또는 이전 학기)이 사전에 제출한 "한 가지 안"일 뿐, 본 과제의 설계 입력이 아니다.
- Claude는 객체지향 설계 원칙(SOLID), 테스트 용이성, NFR에 비추어 **요구사항부터 아키텍처까지 처음부터 재도출**한다.
- 산출물(SRS·SDD·Code 등)에 등장하는 요구사항 ID·UC 이름·클래스 이름·계층 구조·시퀀스는 PDF와 달라도 무방하며, 오히려 **더 단순·명확하게 최적화**되는 것이 목표이다.

> 요약: "PDF·이전 코드는 시스템을 *이해*하기 위한 자료. 요구사항 문장부터 설계·구현까지 Claude가 재증류·재설계한다."

## 3. Tech Stack

- 언어: **C++17** (NFR-OE-01)
- 빌드: **CMake ≥ 3.14** (NFR-OE-01)
- 테스트: **GoogleTest (GTest)**
- 컴파일러: **GCC/G++** (Ubuntu)
- 정적 분석: **cppcheck**, **clang-tidy**
- 커버리지: **lcov** (NFR-O-02 검증 보조)

선택 근거: PDF의 NFR-OE-01(C++17 + CMake 3.14) 강제 조항 + 조교 재현 환경 호환.

## 4. 빌드 환경

**WSL Ubuntu 전용**. Windows 네이티브 빌드는 사용하지 않는다.

Windows에서 명령을 실행할 때는 항상 `wsl` 접두사를 사용한다:

```powershell
wsl bash -lc "ln -sfn '/mnt/c/Users/윤성진/OneDrive/바탕 화면/Konkuk/3-1/객체지향개발방법론/과제/TeamPractice#5/dev/#5_Team7' /tmp/rvc_proj && cmake -S /tmp/rvc_proj -B /tmp/rvc_build && cmake --build /tmp/rvc_build -j && ctest --test-dir /tmp/rvc_build --output-on-failure"
```

> **경로 주의**: 프로젝트 경로에 `#` 문자가 포함되어 있어 Makefile의 주석 처리 충돌이 발생한다.
> `/tmp/rvc_proj` 심볼릭 링크를 통해 우회한다 — cmake는 링크 경로로 Makefile을 생성하므로 재구성 시에도 안전하다.

WSL 진입 시 작업 디렉토리 경로:
```
/mnt/c/Users/윤성진/OneDrive/바탕 화면/Konkuk/3-1/객체지향개발방법론/과제/TeamPractice#5/dev
```

필요 패키지(조교 환경 가정):
```bash
sudo apt update && sudo apt install -y build-essential cmake ninja-build libgtest-dev cppcheck clang-tidy lcov
```

## 5. 디렉토리 규약

```
dev/
├── CLAUDE.md                # 본 파일
├── README.md                # 조교 재현 가이드
├── USAGE.md                 # 팀원용 사용 가이드
├── .claude/commands/        # 9개 슬래시 커맨드 정의
├── docs/
│   ├── specs/               # PDF 추출 명세 (불변, /extract-specs 결과)
│   ├── srs/                 # /srs 결과 (Claude 최적화 설계)
│   ├── sdd/                 # /sdd 결과 (Claude 최적화 설계)
│   └── sa/                  # /sa 정적분석 리포트
├── include/                 # C++ 헤더 (/code 결과)
├── src/                     # C++ 구현 (/code 결과)
├── test/
│   ├── unit/                # /ut 결과 (GTest)
│   ├── system/              # /st 결과
│   └── simulator/           # /simulator 결과 (하드웨어 대체, NFR-O-02)
├── log/                     # 슬래시 커맨드 실행 기록 (발표 자료)
├── reference/               # 명세 PDF + objective.md (수정 금지)
└── CMakeLists.txt           # /code 단계에서 생성
```

## 6. Reference 정책 (중요)

`reference/` 폴더의 구성:
- `객체지향개발방법론#1~#4.pdf` — 팀이 작성한 명세 (요구사항 + 설계 안)
- `objective.md` — 본 과제(TP#5)의 목표
- `CICD-Team7-main/` — 이전 학기 7팀의 코드 (구현 안)

이들은 **"무엇을 만들어야 하는가를 이해하기 위한 1차 자료"**이지, 산출물의 원천 텍스트가 아니다.

### 6.1 허용되는 사용

- **`/extract-specs` 단계에서만 직접 읽는다.** Claude는 PDF·objective.md·이전 코드의 클래스/디렉토리 구조를 **종합적으로 이해**하여 "이 시스템이 무엇을 하는가, 어떤 외부 자극에 어떻게 반응해야 하는가, 어떤 품질 속성을 만족해야 하는가"의 **본질**을 파악한다.
- 파악한 본질을 **재증류**해 `docs/specs/`에 산출한다. 재증류 결과는 PDF와 ID·구조·표현이 달라도 정상이며, 오히려 **더 명료하고 검증 가능한** 형태가 되어야 한다.
- `/srs` 이후 단계는 **재증류된 `docs/specs/`만 입력으로 사용**하고, `reference/`는 다시 보지 않는다.

### 6.2 절대 금지

- PDF의 FR/NFR 표·UC Details 본문·Sequence Diagram·Domain Model 다이어그램의 텍스트나 그림을 **글자 단위·구조 단위로 복제**하여 산출물에 옮기는 것
- 클래스명·메서드명(예: `RVCOrchestrator`, `MovementPolicyController`, `startCleaning()`, `requestStatus()`)을 PDF/이전 코드에서 그대로 가져와 채택하는 것
- 이전 학기 코드(`CICD-Team7-main/`)의 디렉토리 구조·파일 분할·테스트 구성을 그대로 답습하는 것

### 6.3 재증류의 기준

재증류된 요구사항은 다음을 만족해야 한다:

1. **커버리지 완전성**: 원본의 모든 기능·NFR 조항이 어떤 형식으로든 새 사양에 반영되어 있어야 한다. (특히 NFR-OE-01의 C++17/CMake3.14, NFR-O-01의 추상화, NFR-O-02의 Stub 가능성, NFR-P-* 의 시간 한계 등 강제 조항은 반드시 보존)
2. **재구성 가능성**: ID·그룹·표현은 Claude가 더 효율적인 형태로 자유롭게 바꾼다 (예: 중복 NFR 통합, UC 분리/병합, FR과 UC의 매핑 단순화).
3. **추적성**: 재증류 산출물에 "원본의 어느 요구사항을 어떤 새 ID로 흡수했는지" 매핑표를 포함한다. 이것이 표절이 아닌 재해석임을 입증한다.

### 6.4 보호

`reference/` 폴더 내 파일은 **읽기 전용**이다. 절대 수정·이동·삭제하지 않는다.

## 7. 로그 정책 (발표 자료)

**모든 슬래시 커맨드는 첫 동작으로 로그 파일을 만든다.**

- 경로: `log/YYYY-MM-DD_HHMM_<command>.md` (WSL `date +%Y-%m-%d_%H%M` 사용)
- 섹션 (고정):
  1. `# /<command>`
  2. `## 사용 프롬프트` — 사용자가 호출한 슬래시 커맨드 한 줄
  3. `## 주요 결정` — 이 단계에서 내린 설계/구현 판단 (왜를 적는다)
  4. `## 생성·수정 파일` — 절대 경로 리스트
  5. `## 결과 요약` — 산출물의 본질 (3~6줄)
  6. `## 검증 방법` — 빌드/테스트 명령 (조교가 그대로 실행 가능해야 함)
  7. `## 다음 단계` — 권장 후속 슬래시 커맨드

이 기록 자체가 발표 슬라이드로 옮겨진다. 특히 "## 주요 결정"에는 **PDF Domain Model과 다른 설계 선택을 한 이유**를 명시하여 Claude 최적화의 근거를 남긴다.

## 8. 슬래시 커맨드 (산출물 단위)

권장 실행 순서:

1. `/extract-specs` — PDF + objective.md + 이전 코드를 **이해**하고 요구사항을 **재증류** (그대로 옮기지 않음)
2. `/srs` — Software Requirements Specification (재증류 결과 기반 재구성)
3. `/sdd` — Software Design Document (Claude 최적화 설계)
4. `/code` — C++ 구현 (`include/`, `src/`, `CMakeLists.txt`, `main.cpp`)
5. `/ut` — 단위 테스트 (GTest, Stub 활용)
6. `/simulator` — 하드웨어 대체 시뮬레이터 (NFR-O-02)
7. `/st` — 시스템 테스트
8. `/sa` — 정적 분석 (cppcheck + clang-tidy)
9. `/package` — README 최종화 + 재현 점검

상세 정의는 `.claude/commands/<command>.md` 참조.

## 9. 절대 규칙

- 위 디렉토리/파일 규약을 임의로 바꾸지 않는다.
- 외부 의존성을 새로 도입하면 반드시 README의 apt 설치 목록과 CMakeLists.txt에 반영한다.
- 슬래시 커맨드 호출 없이 산출물을 만들지 않는다 (로그 기록 누락 방지).
- `reference/` 폴더의 파일은 절대 수정·이동·삭제하지 않는다.
- **PDF·이전 코드의 텍스트·다이어그램·시그니처를 산출물에 그대로 복제하지 않는다.** 요구사항(FR/NFR/UC) 자체도 Claude가 이해하고 재증류한 형태로 산출하며, 설계·구현은 그 위에서 처음부터 최적화한다.
- `/srs` 이후 단계에서 `reference/`를 다시 열지 않는다. 항상 `/extract-specs`가 만든 재증류 산출물(`docs/specs/`)만을 참조한다.

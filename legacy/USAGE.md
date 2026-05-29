# Claude Code 작업 환경 사용 가이드

본 문서는 RVC Control SW 개발을 Claude Code로 수행하는 **팀원용 가이드**입니다.
조교용 재현 가이드는 [README.md](README.md), AI 운영 규약은 [CLAUDE.md](CLAUDE.md)를 참고하세요.

---

## 1. 환경 개요

```
사용자 입력 (슬래시 커맨드)
        ↓
Claude Code 세션 (자동 로드: CLAUDE.md)
        ↓
.claude/commands/<command>.md 의 절차 실행
        ↓
산출물 생성 (docs/, include/, src/, test/) + log/ 자동 기록
        ↓
WSL Ubuntu 에서 빌드/테스트 검증
```

**핵심 규약** (세 가지만 기억하면 됨):
1. 모든 작업은 **슬래시 커맨드 호출**로 시작한다 — 직접 코딩 요청 금지 (로그 누락 방지).
2. **`reference/` 폴더는 "이해의 입력"이지 산출물의 원본이 아니다.** PDF·objective.md·이전 코드(`CICD-Team7-main/`)는 `/extract-specs` 단계에서 통독하여 시스템의 의도를 파악하는 데만 쓴다. **요구사항(FR/NFR/UC) 자체도 Claude가 재증류**하며, PDF 표·문장·다이어그램·클래스명·메서드명을 그대로 옮기지 않는다. `/srs` 이후 단계는 `docs/specs/`(재증류 결과)만 참조한다.
3. 빌드·실행은 **WSL Ubuntu** 전용. Windows 네이티브 빌드 사용 안 함.

---

## 2. 최초 1회 환경 준비

### 2-1. WSL Ubuntu 패키지 설치

PowerShell에서 한 번만 실행:

```powershell
wsl bash -lc "sudo apt update && sudo apt install -y build-essential cmake ninja-build libgtest-dev cppcheck clang-tidy lcov"
```

설치 확인:

```powershell
wsl bash -lc "which g++ cmake ctest cppcheck clang-tidy lcov && g++ --version | head -1 && cmake --version | head -1"
```

### 2-2. Claude Code 세션 확인

새 Claude Code 세션을 시작하고 다음을 확인:

- 슬래시 커맨드 자동완성에 `/extract-specs`, `/srs`, `/sdd`, `/code`, `/ut`, `/simulator`, `/st`, `/sa`, `/package`가 모두 노출되는지
- `CLAUDE.md`가 자동 로드되어 컨텍스트에 들어왔는지 (Claude에게 "현재 프로젝트 정책이 뭐야?" 물어 확인 가능)
- Claude가 "reference는 이해 자료이며 FR/NFR/UC도 재증류한다"는 정책을 인지하는지

---

## 3. 권장 호출 순서 (전체 워크플로우)

```
1. /extract-specs   → docs/specs/*.md (Claude 재증류: system-overview, requirements, use-cases + 추적성 표)
2. /srs             → docs/srs/SRS.md (재증류 결과 기반 SRS)
3. /sdd             → docs/sdd/SDD.md (Claude 최적화 설계)
4. /code            → include/, src/, CMakeLists.txt, main.cpp
5. /ut              → test/unit/*.cpp
6. /simulator       → test/simulator/*
7. /st              → test/system/*.cpp
8. /sa              → docs/sa/{cppcheck,clang-tidy}.txt + summary.md
9. /package         → README/INDEX 최종화, 클린 빌드 검증
```

각 단계는 **다음 단계의 입력을 만든다**. 순서를 건너뛰면 후속 커맨드가 입력 부족으로 실패한다.

### 단계별 소요 시간 (예상)

| 단계 | 예상 시간 | 한 세션에서 끝낼만함 |
|---|---|---|
| /extract-specs | 10~20분 | ✅ |
| /srs | 15~25분 | ✅ |
| /sdd | 20~30분 | ✅ |
| /code | 30~60분 | △ (큰 작업, 분할 권장) |
| /ut | 30~45분 | ✅ |
| /simulator | 15~25분 | ✅ |
| /st | 25~40분 | ✅ |
| /sa | 10~20분 | ✅ |
| /package | 10~15분 | ✅ |

---

## 4. 각 단계 사용법과 체크포인트

### `/extract-specs` — 명세 재증류

```
호출: /extract-specs
입력: reference/ 의 PDF 4개 + objective.md + CICD-Team7-main/ (통독·이해 목적)
출력: docs/specs/{system-overview.md, requirements.md, use-cases.md}
```

**완료 체크**:
- [ ] `docs/specs/`에 3개 문서(system-overview, requirements, use-cases) 존재
- [ ] `requirements.md`에 새 ID 체계(예: FR-MOTION-01, NFR-TIMING-01)로 재진술되어 있음
- [ ] `requirements.md` 끝에 원본 ID ↔ 새 ID 추적성 표 존재
- [ ] 원본의 강제 조항(C++17/CMake3.14, 50ms/100ms/500ms/5초 시간 한계, 추상화·Stub, 우선순위·무시 규칙)이 새 사양에서 모두 추적 가능
- [ ] PDF 표·문장을 그대로 복사한 흔적이 없고, RVCOrchestrator·MovementPolicyController 같은 PDF 클래스명이 산출물에 등장하지 않음
- [ ] `log/YYYY-MM-DD_HHMM_extract-specs.md`에 자기 점검 결과 포함

---

### `/srs` — Software Requirements Specification

```
호출: /srs
입력: docs/specs/ (외 자료 보지 않음)
출력: docs/srs/SRS.md
```

**완료 체크**:
- [ ] FR-001, FR-002, ... NFR-001, ... 형식의 ID 부여됨
- [ ] 유스케이스 표 + 추적성 매트릭스 존재
- [ ] mermaid context diagram 1개 이상 (Claude가 새로 그린 것)

---

### `/sdd` — Software Design Document (Claude 최적화)

```
호출: /sdd
입력: docs/srs/SRS.md
출력: docs/sdd/SDD.md
```

**완료 체크**:
- [ ] 클래스 분해·책임 할당이 PDF Domain Model과 동일하지 않음 — Claude가 SOLID/테스트 용이성/NFR 관점에서 재설계함
- [ ] 컴포넌트 시그니처가 C++ 코드블록으로 작성됨 (/code 단계가 그대로 받아씀)
- [ ] 시퀀스 다이어그램 3~5개 (Claude가 새로 그린 것)
- [ ] 디렉토리·파일 매핑 표 존재
- [ ] 로그의 `## 주요 결정`에 **PDF와 다른 설계 선택의 근거**가 명시됨

---

### `/code` — C++ 구현

```
호출: /code
입력: docs/sdd/SDD.md
출력: include/, src/, main.cpp, CMakeLists.txt
검증: WSL에서 cmake 빌드 성공 + 경고 0
```

**완료 체크**:
- [ ] `wsl bash -lc "cmake -S . -B build && cmake --build build -j"` 성공
- [ ] 빌드 경고 0개
- [ ] `build/oop` 실행파일 생성됨

**주의**: 이 단계가 가장 길다. 중간에 세션이 길어지면 `/code` 안에서 "여기까지 컨트롤러 A, B 완료, 다음 세션에서 C부터" 식으로 로그에 상태를 적고 다음 세션에서 이어간다.

---

### `/ut` — 단위 테스트

```
호출: /ut
입력: include/, src/, docs/sdd/SDD.md
출력: test/unit/*.cpp
검증: ctest로 단위 테스트 전체 통과
```

**완료 체크**:
- [ ] `ctest --test-dir build -R unit_tests --output-on-failure` 전체 통과
- [ ] NFR-O-02 충족: Stub 객체로 외부 의존성 없이 테스트 가능
- [ ] 각 클래스마다 Positive + Negative 테스트 존재
- [ ] (선택) lcov 커버리지 측정 결과가 로그에 기록됨

---

### `/simulator` — 시뮬레이터

```
호출: /simulator
입력: include/ 의 하드웨어 추상화 인터페이스 (NFR-O-01)
출력: test/simulator/{Sim*, RVCSimulator}
검증: oop_system_test 타겟 빌드 성공
```

**완료 체크**:
- [ ] 환경 주입 API 존재 (장애물·먼지 등의 외부 상태를 테스트에서 제어)
- [ ] 상태 쿼리 API 존재 (모터 동작·청소 모드 등을 테스트에서 확인)
- [ ] 빌드 성공 (`cmake --build build --target oop_system_test`)

---

### `/st` — 시스템 테스트

```
호출: /st
입력: test/simulator/, docs/srs/SRS.md (UC)
출력: test/system/*.cpp
검증: ctest로 시스템 테스트 전체 통과
```

**완료 체크**:
- [ ] `ctest --test-dir build -R system_tests --output-on-failure` 전체 통과
- [ ] 시나리오마다 검증 대상 UC ID 주석 존재

---

### `/sa` — 정적 분석

```
호출: /sa
입력: build/compile_commands.json + 전체 소스
출력: docs/sa/{cppcheck.txt, clang-tidy.txt, summary.md}
```

**완료 체크**:
- [ ] cppcheck error/warning 0
- [ ] clang-tidy 잔여 이슈는 summary.md에 사유 명시
- [ ] summary.md에 심각도별 카운트 표 존재

---

### `/package` — 최종 패키징

```
호출: /package
입력: 전체 산출물
출력: README.md 갱신, log/INDEX.md, 클린 빌드 검증 로그
```

**완료 체크**:
- [ ] `rm -rf build && cmake -S . -B build && cmake --build build && ctest` 한 줄 명령 통과
- [ ] `log/INDEX.md`에 모든 슬래시 커맨드 실행 기록이 시간순으로 링크됨
- [ ] README의 산출물 표가 실제 파일과 일치

---

## 5. 작업 중단·재개

긴 작업 도중 세션이 끊기거나 다음 날 이어가는 경우:

1. **이전 로그 확인**: `log/` 폴더에서 가장 최근 마크다운 파일 열어 "## 다음 단계" 확인.
2. **새 세션 시작**: Claude Code 새 세션 시작 → CLAUDE.md 자동 로드.
3. **이어갈 커맨드 호출**: "다음 단계"에 적힌 슬래시 커맨드 그대로 호출.

같은 커맨드를 다시 호출해도 됨 — 로그가 새 타임스탬프로 생성되므로 기록은 분리됨.

---

## 6. 발표 자료 활용

`log/*.md` 파일은 그대로 발표 슬라이드 소스가 된다. 추천 사용법:

- **결정 흐름**: 각 로그의 "## 주요 결정" 섹션만 모으면 의사결정 타임라인 완성. 특히 `/sdd`·`/code` 로그에는 **PDF 안과 다르게 최적화한 근거**가 담겨 AI 활용의 핵심 증거가 된다.
- **AI 활용 증거**: "## 사용 프롬프트" 섹션으로 어떤 프롬프트가 어떤 산출물을 만들었는지 추적 가능.
- **재현성 증거**: "## 검증 방법" 섹션의 명령어를 발표 중 실시간 시연.

`/package` 단계가 만들어주는 `log/INDEX.md`를 발표 자료의 목차로 그대로 사용.

---

## 7. 자주 묻는 질문

**Q. Claude가 PDF의 R1.1·NFR-O-06 같은 원본 ID를 산출물 본문에서 그대로 사용한다.**
A. CLAUDE.md 2·6절 정책 위반. 원본 ID는 `/extract-specs`의 추적성 표 안에서만 사용되어야 하고, 본문은 새 ID(FR-MOTION-01 등)만 사용해야 한다. "원본 ID 사용을 추적성 표로만 제한하고 본문은 재증류된 새 ID로 다시 써" 라고 정정 지시.

**Q. Claude가 PDF Domain Model의 클래스명(RVCOrchestrator 등)을 그대로 SDD에 옮기려고 한다.**
A. CLAUDE.md 6절 정책 위반. "PDF Domain Model과 CICD-Team7-main 코드 구조는 사용하지 말고 docs/specs/의 재증류 결과만 보고 Claude가 새로 설계해" 라고 정정 지시. 결과 클래스 이름·계층이 달라도 정상이다.

**Q. 빌드가 깨졌는데 어디서 고쳐야 하나?**
A. 빌드 오류는 `/code` 단계에서 즉시 수정해야 함. 다른 단계로 넘어가지 말 것. 같은 세션에서 "빌드 오류: <오류 메시지>. /code 단계 규약대로 수정해" 라고 추가 요청.

**Q. 슬래시 커맨드를 안 쓰고 그냥 코드 짜달라고 해도 되나?**
A. 안 됨. 로그가 누락되면 발표 자료가 비고, "AI 일관 활용" 목표를 입증할 수 없음. 항상 슬래시 커맨드부터 호출.

**Q. PDF 통독이 너무 오래 걸린다.**
A. `/extract-specs` 안에서 PDF Read tool의 `pages` 파라미터로 페이지 범위(예: `1-5`)를 잘게 나눠 읽도록 Claude에게 추가 지시. Domain Model·Sequence Diagram이 그려진 페이지는 빠르게 훑고 그대로 옮기지 말 것. 통독의 목표는 "시스템 의도 파악"이지 "텍스트 변환"이 아님을 환기.

**Q. 재증류 결과가 원본보다 더 작아도 되나?**
A. 권장됨. 중복·과세분화된 요구사항을 통합하고 묵시적 가정을 명시화하면 결과는 더 짧고 검증 가능해진다. 단, 추적성 표로 원본의 모든 항목이 어떤 새 ID에 흡수되었는지 명시되어야 한다.

**Q. WSL이 없는 환경(예: 학교 실습실)에서 검증해야 한다.**
A. 조교용 재현은 README.md에 적힌 Linux 명령으로 가능. WSL은 Windows 개발자 편의용.

---

## 8. 한눈 요약 (북마크용)

```
첫 회: wsl 패키지 설치 → 새 세션 → /extract-specs
일상: 직전 로그의 "다음 단계" 확인 → 해당 슬래시 커맨드 호출
빌드: wsl bash -lc "cmake -S . -B build && cmake --build build && ctest --test-dir build"
점검: log/INDEX.md (제일 마지막에 /package가 생성)
설계 원칙: reference는 이해의 입력. FR/NFR/UC도 재증류, Domain Model·Sequence Diagram·이전 코드 구조는 산출물에 옮기지 않음
```

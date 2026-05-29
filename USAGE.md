# Claude Code 작업 환경 사용 가이드 (TP#6 유지보수)

본 문서는 RVC Control SW **유지보수**(오른쪽 근접 센서 삭제, 센서 3→2)를 Claude Code로 수행하는 **팀원용 가이드**입니다.
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
legacy/<아티팩트> 인계 → 오른쪽 센서 삭제 최소 적용 → docs/·src/·test/ 에 수정본 + log/ 자동 기록
        ↓
WSL Ubuntu 에서 빌드/테스트 검증
```

**핵심 규약** (세 가지만 기억하면 됨):
1. 모든 작업은 **슬래시 커맨드 호출**로 시작한다 — 직접 코딩 요청 금지 (로그 누락 방지).
2. **유지보수의 기준선은 `legacy/`다.** 바닥부터 다시 만들지 않고, `legacy/`를 인계받아 오른쪽 센서 삭제가 강제하는 부분만 최소 수정한다. **TP#5가 참고했던 원본(PDF, `CICD-Team7-main/`, `objective.md`)은 절대 열지 않는다.** 모든 수정은 `legacy/` 대비 **변경/일치**를 명시한다.
3. 빌드·실행은 **WSL Ubuntu** 전용. Windows 네이티브 빌드 사용 안 함.

---

## 2. 최초 1회 환경 준비

### 2-1. WSL Ubuntu 패키지 설치

PowerShell에서 한 번만 실행:

```powershell
wsl bash -lc "sudo apt update && sudo apt install -y build-essential cmake ninja-build libgtest-dev cppcheck clang-tidy lcov"
```

### 2-2. 기준선·세션 확인

- `legacy/`에 TP#5 산출물(`legacy/docs`, `legacy/include`, `legacy/src`, `legacy/test`, `legacy/CMakeLists.txt`)이 읽기전용으로 존재하는지
- 슬래시 커맨드 자동완성에 `/change-impact`, `/srs`, `/sdd`, `/code`, `/ut`, `/simulator`, `/st`, `/sa`, `/package`가 노출되는지 (`/extract-specs`는 제거됨)
- Claude가 "기준선은 legacy, 원본(PDF/CICD/objective.md)은 열람 금지, 차이 명시 의무"라는 정책을 인지하는지

---

## 3. 권장 호출 순서 (유지보수 워크플로우)

```
1. /change-impact   → docs/maintenance/impact.md (오른쪽 센서 삭제 영향 범위 + 일치 예정 목록)
2. /srs             → docs/srs/SRS.md (legacy 인계 후 수정 + 변경/일치 표)
3. /sdd             → docs/sdd/SDD.md (legacy 인계 후 수정 + 변경/일치 표)
4. /code            → include/, src/, CMakeLists.txt (legacy 인계 후 수정 + 빌드)
5. /ut              → test/unit/* (legacy 인계 후 수정 + ctest)
6. /simulator       → test/simulator/* (legacy 인계 후 수정)
7. /st              → test/system/* (legacy 인계 후 수정 + ctest)
8. /sa              → docs/sa/* (수정 코드 재분석, legacy 리포트 대비 비교)
9. /package         → docs/maintenance/diff-matrix.md + README/INDEX 최종화 + 클린 빌드
```

각 단계는 **다음 단계의 입력을 만든다**. `/change-impact`가 만든 `impact.md`가 이후 모든 단계의 스코프 기준이다.

---

## 4. 각 단계 사용법과 체크포인트

### `/change-impact` — 변경 영향 분석 (진입점)

```
호출: /change-impact
입력: reference/new_objective.md + legacy/ 전체
출력: docs/maintenance/impact.md
```

**완료 체크**:
- [ ] 오른쪽 센서가 legacy의 코드·테스트·문서 어디에 닿는지 매트릭스로 정리됨
- [ ] **"일치 예정" 목록**(예: 방향 비특정 근접 센서 인터페이스)이 명시됨
- [ ] 각 후속 단계가 건드릴 파일 목록이 매트릭스에 있음
- [ ] PDF·CICD·objective.md를 열지 않음

---

### `/srs`, `/sdd` — 문서 유지보수

```
입력: legacy/docs/{srs,sdd}/... + impact.md (+ 상위 단계 수정본)
출력: docs/{srs,sdd}/... (legacy 인계 후 수정)
```

**완료 체크**:
- [ ] legacy 문서를 출발점으로 삼고, 오른쪽 센서와 무관한 부분을 재작성하지 않음
- [ ] 센서 3→2·회피 로직 단순화가 일관 반영됨
- [ ] 문서 말미에 **변경/일치 이력 표** 존재 (일치 항목도 명시)

---

### `/code` — 구현 유지보수

```
입력: legacy/{include,src,CMakeLists.txt} + docs/sdd/SDD.md + impact.md
출력: include/, src/, CMakeLists.txt
검증: WSL 클린 빌드 성공 + 경고 0
```

**완료 체크**:
- [ ] `wsl bash -lc "cmake -S . -B build && cmake --build build -j"` 성공, 경고 0
- [ ] 우측 센서 참조가 main·엔진·회피 핸들러에서 제거됨
- [ ] 방향 비특정 인터페이스 등 변경 불필요 파일은 legacy 그대로(="일치")
- [ ] `diff -rq legacy/src src` 로 변경/동일 파일 집합 확인

---

### `/ut`, `/st` — 테스트 유지보수

```
입력: legacy/test/{unit,system} + 수정 구현/하니스
출력: test/{unit,system}/*
검증: ctest 전체 통과
```

**완료 체크**:
- [ ] 우측 센서 케이스·우회전 기대값이 일관되게 수정됨
- [ ] 센서 무관 테스트/시나리오는 legacy 그대로(="일치"), UC 커버리지 유지
- [ ] 해당 ctest 타겟 전체 통과

---

### `/simulator` — 시뮬레이터 유지보수

```
입력: legacy/test/simulator + 수정 인터페이스
출력: test/simulator/*
검증: oop_system_test 타겟 빌드 성공
```

**완료 체크**:
- [ ] 환경 주입 API에서 우측 센서 주입 경로 제거
- [ ] Sim/하니스 이름·구조 보존
- [ ] 시스템 테스트 타겟 빌드 성공

---

### `/sa` — 정적 분석 재실행

```
입력: 수정된 src/include + build/compile_commands.json
출력: docs/sa/{cppcheck.txt, clang-tidy.txt, summary.md}
```

**완료 체크**:
- [ ] 수정 코드 기준 재분석 (legacy 아님)
- [ ] `legacy/docs/sa` 대비 이슈 증감 명시
- [ ] 새로 생긴 error/warning 처리

---

### `/package` — 최종 패키징

```
입력: 전체 수정 산출물 + 각 로그의 ## 차이 표기
출력: docs/maintenance/diff-matrix.md, README.md, log/INDEX.md, 클린 빌드 로그
```

**완료 체크**:
- [ ] `rm -rf build && cmake -S . -B build && cmake --build build && ctest` 한 줄 통과
- [ ] `docs/maintenance/diff-matrix.md`에 전 아티팩트 변경/일치가 통합됨 (일치 항목 포함)
- [ ] `log/INDEX.md`에 모든 커맨드 기록이 시간순 링크 + 차이 1줄 요약

---

## 5. 작업 중단·재개

1. **이전 로그 확인**: `log/`에서 최근 마크다운 파일의 "## 다음 단계" 확인.
2. **새 세션 시작**: CLAUDE.md 자동 로드.
3. **이어갈 커맨드 호출**: "다음 단계"의 슬래시 커맨드 호출.

---

## 6. 발표 자료 활용

`log/*.md`가 발표 슬라이드 소스다. 본 과제의 핵심은 각 로그의 **`## 차이 표기`** 섹션 — "오른쪽 센서 삭제로 무엇이 바뀌고 무엇이 그대로인지"를 모으면 유지보수 의사결정 타임라인이 완성된다. `/package`가 만드는 `docs/maintenance/diff-matrix.md`와 `log/INDEX.md`를 발표 목차로 사용.

---

## 7. 자주 묻는 질문

**Q. Claude가 PDF·`CICD-Team7-main/`·`objective.md`를 열려고 한다.**
A. CLAUDE.md 6절 정책 위반. "원본 출처는 열람 금지다. 입력은 `legacy/`와 `new_objective.md`뿐이다"라고 정정 지시.

**Q. Claude가 오른쪽 센서와 무관한 부분까지 재작성/재설계하려 한다.**
A. CLAUDE.md 2절 위반. "바닥부터 재설계 금지. `legacy/`를 인계받아 오른쪽 센서 삭제가 강제하는 부분만 최소 수정하고, 나머지는 legacy 그대로 두고 '일치'로 표기해"라고 정정.

**Q. 변경/일치를 어떻게 검증하나?**
A. `diff -rq legacy/<dir> <dir>`로 변경/동일 파일 집합을 기계적으로 확인하고, 각 로그 `## 차이 표기`와 최종 `diff-matrix.md`에 근거로 남긴다.

**Q. 빌드가 깨졌다.**
A. `/code` 단계에서 즉시 수정. 원인이 SDD 모호함이면 `/sdd`로 돌아가 보강 후 재진행.

**Q. 슬래시 커맨드 없이 그냥 고쳐달라고 해도 되나?**
A. 안 됨. 로그·차이 표기가 누락되면 채점 핵심(변경/일치 명시)을 입증할 수 없음.

---

## 8. 한눈 요약 (북마크용)

```
첫 회: wsl 패키지 설치 → legacy/ 확인 → 새 세션 → /change-impact
일상: 직전 로그의 "다음 단계" 확인 → 해당 슬래시 커맨드 호출
빌드: wsl bash -lc "cmake -S . -B build && cmake --build build && ctest --test-dir build"
차이: diff -rq legacy/<dir> <dir>  →  docs/maintenance/diff-matrix.md
원칙: 기준선=legacy, 원본(PDF/CICD/objective.md) 열람 금지, 최소 수정, 변경/일치 명시
```

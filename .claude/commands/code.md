---
description: legacy/{include,src,CMakeLists.txt}를 인계받아 오른쪽 센서 삭제를 반영한 C++17 구현을 만들고 WSL에서 빌드를 검증한다. legacy 대비 변경/일치를 명시한다.
---

# /code — 구현 유지보수 (오른쪽 센서 삭제 반영)

> **정책 (CLAUDE.md 2·6·7절)**
> 바닥부터 재구현하지 않는다. `legacy/include`·`legacy/src`·`legacy/CMakeLists.txt`를 그대로 인계받아 **수정된 SDD가 지시하는 부분만** 고친다. 파일 분할·클래스명·디렉토리는 legacy 그대로.
> PDF·`CICD-Team7-main/`·`objective.md`는 열람 금지. 수정 코드는 legacy 대비 **변경/일치**를 명시한다.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /code" > log/${TS}_code.md
echo "" >> log/${TS}_code.md
echo "## 사용 프롬프트" >> log/${TS}_code.md
echo "\`/code\`" >> log/${TS}_code.md
```

## 1. 입력 (이외 자료 보지 말 것)

- `legacy/include/`, `legacy/src/`, `legacy/CMakeLists.txt` — 기준선 구현 (인계 대상)
- `docs/sdd/SDD.md` — 방금 수정된 SDD (단일 진실 소스)
- `docs/maintenance/impact.md` — Code 계층 변경 스코프

`reference/`(PDF·CICD·objective.md), `docs/specs/`는 열람 금지.

## 2. 작업 — 인계 후 최소 수정

1. `legacy/include`·`legacy/src`·`legacy/CMakeLists.txt`를 최상위 `include/`·`src/`·`CMakeLists.txt`로 인계한다 (출발점은 legacy 그대로).
2. 수정된 SDD와 `impact.md`가 지목한 파일만 고친다. 우측 센서 삭제의 전형적 변경점:
   - **컴포지션 루트(main)**: 우측 센서 인스턴스 제거, 엔진/핸들러 생성 인자에서 우측 센서 제거.
   - **엔진 ctor**: 우측 근접 센서 인자 제거.
   - **회피 핸들러**: 우측 센서 멤버·참조 제거, 경로 선택 로직을 "좌측 가능 → 좌회전, 아니면 후진"으로 단순화 (우회전·후진후재탐색의 우측 분기 정리).
   - **방향 비특정 근접 센서 인터페이스**: 변경 불필요 → **일치**.
3. 클래스명·메서드명·파일 위치는 SDD(=legacy)가 정한 그대로. SDD에 없는 새 파일을 만들지 않는다.

## 3. 빌드 검증 (WSL)

```bash
wsl bash -lc "cd '$(pwd)' && rm -rf build && cmake -S . -B build && cmake --build build -j 2>&1 | tee /tmp/rvc_build.log"
```

- C++17, `-Wall -Wextra -Wpedantic` **경고 0**.
- 빌드 실패·경고가 남으면 본 단계 미완료. 단, SDD 모호함이 원인이면 임시방편 대신 `/sdd`로 돌아가 보강 후 재진행.

## 4. 차이 표기 (필수)

변경된 모든 파일을 legacy와 대조한다:

| 파일 | legacy 대비 | 내용 |
|---|---|---|
| `src/app/main.cpp` | 변경 | 우측 센서 인스턴스·전달 제거 |
| `src/core/obstacle_handler.cpp` | 변경 | 우측 분기 제거, 경로 선택 단순화 |
| `include/interface/i_proximity_sensor.hpp` | 일치 | 방향 비특정, legacy 그대로 |
| ... | ... | ... |

`wsl bash -lc "diff -rq legacy/include include; diff -rq legacy/src src"`로 변경/동일 파일 집합을 확인하고 표를 채운다.

## 5. 자기 점검

- [ ] legacy의 파일 분할·클래스명·디렉토리를 보존했는가? (새 파일·재명명 없음)
- [ ] 우측 센서 참조가 main·엔진·핸들러에서 일관되게 제거됐는가?
- [ ] 변경 불필요한 파일(예: 방향 비특정 인터페이스)을 "일치"로 명시했는가?
- [ ] 빌드 경고 0, 클린 빌드 성공인가?
- [ ] PDF·CICD·objective.md, docs/specs를 열지 않았는가?

## 6. 로그 종료

`log/${TS}_code.md`에 채움:
- `## 주요 결정`: 우측 센서 제거를 어느 파일에서 어떻게 반영했는지, 회피 로직 단순화 구현, 보존한 구조
- `## 생성·수정 파일`: 변경된 .h/.cpp/CMakeLists.txt 경로
- `## 차이 표기`: legacy 대비 변경 파일 N개 / 일치 파일 M개 (diff -rq 근거)
- `## 결과 요약`: 변경 파일 수, 삭제 LOC, 빌드 경고 수(0)
- `## 검증 방법`: 위 클린 빌드 명령
- `## 다음 단계`: `/ut`

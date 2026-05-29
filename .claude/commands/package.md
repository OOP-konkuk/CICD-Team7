---
description: 유지보수 산출물의 일관성·재현성을 점검하고, 전 아티팩트 통합 diff 매트릭스(docs/maintenance/diff-matrix.md)를 만들고 README를 최종화한다. 조교가 한 줄 명령으로 빌드/테스트할 수 있는 상태로 마무리.
---

# /package — 최종 패키징 & 재현성 점검 (유지보수)

> **정책 (CLAUDE.md 2·6·7절)**
> 본 단계는 유지보수 결과를 마무리하고, **legacy 대비 전 아티팩트 변경/일치를 한 곳에 통합**한다.
> PDF·CICD·objective.md는 열람 금지.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /package" > log/${TS}_package.md
echo "" >> log/${TS}_package.md
echo "## 사용 프롬프트" >> log/${TS}_package.md
echo "\`/package\`" >> log/${TS}_package.md
```

## 1. 산출물 점검 체크리스트

| 항목 | 위치 | 점검 |
|---|---|---|
| 변경영향 | `docs/maintenance/impact.md` | 존재 + 영향 매트릭스 |
| SRS | `docs/srs/SRS.md` | 센서 2개 반영 + 변경 이력 표 |
| SDD | `docs/sdd/SDD.md` | 우측 센서 의존성 제거 + 변경 이력 표 |
| Code | `include/`, `src/`, `CMakeLists.txt` | 빌드 경고 0, 우측 센서 제거 |
| UT | `test/unit/*` | 전부 통과 |
| Simulator | `test/simulator/*` | 우측 주입 API 제거 |
| ST | `test/system/*` | 전부 통과, UC 커버리지 유지 |
| SA | `docs/sa/*` | 3개 파일 + legacy 대비 비교 |
| 로그 | `log/*.md` | 각 커맨드 1개 이상 + `## 차이 표기` 채움 |
| README | `README.md` | 한 줄 명령 동작 + legacy 비교 안내 |

## 2. 클린 빌드 재현 (WSL)

```bash
wsl bash -lc "cd '$(pwd)' && rm -rf build && cmake -S . -B build && cmake --build build -j 2>&1 | tee /tmp/rvc_package_build.log && ctest --test-dir build --output-on-failure 2>&1 | tee /tmp/rvc_package_test.log"
```

빌드 경고 0, 테스트 100% 통과 확인.

## 3. 통합 diff 매트릭스 — `docs/maintenance/diff-matrix.md` (본 과제 핵심 산출물)

각 단계 로그의 `## 차이 표기`를 한 표로 통합한다:

| 아티팩트 | 파일 | legacy 대비 | 변경 요지 |
|---|---|---|---|
| SRS | `docs/srs/SRS.md` | 변경 | 센서 3→2, 회피 FR 단순화 |
| SDD | `docs/sdd/SDD.md` | 변경 | 우측 센서 의존성·시퀀스 제거 |
| Code | `src/core/obstacle_handler.cpp` | 변경 | 우측 분기 제거 |
| Code | `include/interface/i_proximity_sensor.hpp` | 일치 | 방향 비특정, legacy 그대로 |
| ... | ... | ... | ... |

- **변경/일치 모두 빠짐없이** 적는다. "일치"가 무엇인지 보이는 것이 채점 핵심.
- 기계적 근거 보강:
  ```bash
  wsl bash -lc "cd '$(pwd)' && for d in docs include src test CMakeLists.txt; do echo \"=== \$d ===\"; diff -rq legacy/\$d \$d 2>/dev/null; done"
  ```

## 4. README / USAGE 최종화

- 유지보수 워크플로우(`/change-impact` → … → `/package`)와 일치하는지
- `legacy/` 기준선 + 수정본 비교 방법 안내가 있는지
- 한 줄 빌드/테스트 명령이 §2와 일치하는지
- apt 패키지 목록이 legacy와 동일(변경 없음)한지
- `README.md`에 **빌드/테스트 결과 요약**(`/tmp/rvc_package_*.log` 통계)과 **AI 작동 기록 인덱스**(`log/` 링크) 갱신

## 5. 발표 자료 인덱스

`log/INDEX.md`를 갱신:
- 커맨드 실행 순서별 로그 링크 (`/change-impact`부터)
- 각 로그의 `## 차이 표기` 1줄 요약 — 발표자가 "무엇이 바뀌고 무엇이 그대로인지"를 한눈에

## 6. 로그 종료

`log/${TS}_package.md`에 채움:
- `## 주요 결정`: 최종 점검 보완 항목, README 변경 요점
- `## 생성·수정 파일`: README.md, USAGE.md, log/INDEX.md, docs/maintenance/diff-matrix.md
- `## 차이 표기`: 전 아티팩트 통합 — 변경 파일 총 N개 / 일치 파일 총 M개
- `## 결과 요약`: 클린 빌드 시간, 테스트 통과율, legacy 대비 총 변경/일치 집계
- `## 검증 방법`: §2 클린 빌드 + §3 diff -rq 명령
- `## 다음 단계`: 제출

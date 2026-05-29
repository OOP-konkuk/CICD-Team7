---
description: legacy/test/unit을 인계받아 오른쪽 센서 삭제를 반영한 단위 테스트(GTest)를 수정하고 ctest를 실행한다. legacy 대비 변경/일치를 명시한다.
---

# /ut — 단위 테스트 유지보수 (오른쪽 센서 삭제 반영)

> **정책 (CLAUDE.md 2·6·7절)**
> 테스트를 새로 짜지 않는다. `legacy/test/unit`을 인계받아 **우측 센서 삭제로 깨지거나 무의미해진 케이스·스텁만** 수정한다.
> PDF·CICD·objective.md는 열람 금지. 수정 테스트는 legacy 대비 **변경/일치**를 명시한다.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /ut" > log/${TS}_ut.md
echo "" >> log/${TS}_ut.md
echo "## 사용 프롬프트" >> log/${TS}_ut.md
echo "\`/ut\`" >> log/${TS}_ut.md
```

## 1. 입력 (이외 자료 보지 말 것)

- `legacy/test/unit/` — 기준선 단위 테스트 + 스텁 (인계 대상)
- `include/`, `src/` — 방금 수정된 구현 (테스트 단위의 진실 소스)
- `docs/sdd/SDD.md`, `docs/srs/SRS.md` — 수정된 설계/요구 (검증 기준)
- `docs/maintenance/impact.md` — UT 계층 변경 스코프

`reference/`(PDF·CICD·objective.md), `docs/specs/`는 열람 금지.

## 2. 작업 — 인계 후 최소 수정

1. `legacy/test/unit/`을 최상위 `test/unit/`로 인계한다.
2. 수정된 구현에 맞춰 영향받는 부분만 고친다:
   - 우측 센서 스텁(예: 근접 센서 스텁의 우측 인스턴스 사용)·우측 분기 검증 케이스 제거/수정.
   - 회피 핸들러 테스트: 경로 선택이 "좌측 가능 → 좌회전, 아니면 후진"으로 단순화된 것을 검증하도록 기대값 조정.
   - 엔진 테스트: 생성 인자에서 우측 센서 제거 반영.
3. 테스트 파일명·구조는 **실제 수정 구현의 클래스 구조(=legacy 구조)를 그대로 따른다**. 우측 센서와 무관한 케이스는 legacy 그대로 둔다.

## 3. 빌드·실행 검증 (WSL)

```bash
wsl bash -lc "cd '$(pwd)' && cmake --build build -j --target oop_test && ctest --test-dir build -R unit_tests --output-on-failure"
```

모든 테스트 통과 + 빌드 경고 0 일 때 완료.

## 4. 차이 표기 (필수)

| 테스트 파일/케이스 | legacy 대비 | 내용 |
|---|---|---|
| `test_obstacle_handler.cpp` | 변경 | 우회전 경로 케이스 삭제, 좌/후진 분기로 단순화 |
| `stub/stub_proximity_sensor.hpp` | 일치/변경 | (방향 비특정이면 일치) |
| `test_clean_intensity_controller.cpp` | 일치 | 센서 무관, legacy 그대로 |
| ... | ... | ... |

`wsl bash -lc "diff -rq legacy/test/unit test/unit"`로 변경/동일 집합 확인.

## 5. 자기 점검

- [ ] 테스트 구조가 수정 구현(=legacy 구조)과 일치하는가?
- [ ] 우측 센서 관련 케이스가 일관되게 제거/수정됐는가?
- [ ] 센서 무관 테스트를 "일치"로 명시했는가?
- [ ] 전체 통과 + 경고 0인가?
- [ ] Production 코드에 스텁이 섞이지 않았는가?

## 6. 로그 종료

`log/${TS}_ut.md`에 채움:
- `## 주요 결정`: 어떤 케이스를 왜 수정/삭제했는지, 회피 단순화 검증 방식
- `## 생성·수정 파일`: 변경된 테스트/스텁 파일
- `## 차이 표기`: legacy 대비 변경 N건 / 일치 M건
- `## 결과 요약`: 테스트 개수(전/후), 통과율
- `## 검증 방법`: 위 ctest 명령
- `## 다음 단계`: `/simulator`

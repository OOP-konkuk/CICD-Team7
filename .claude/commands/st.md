---
description: legacy/test/system을 인계받아 오른쪽 센서 삭제를 반영한 시스템 테스트(GTest)를 수정하고 실행한다. legacy 대비 변경/일치를 명시한다.
---

# /st — 시스템 테스트 유지보수 (오른쪽 센서 삭제 반영)

> **정책 (CLAUDE.md 2·6·7절)**
> 시나리오를 새로 짜지 않는다. `legacy/test/system`을 인계받아 **우측 센서 삭제로 영향받는 회피 시나리오만** 수정한다.
> PDF·CICD·objective.md는 열람 금지. 수정 시스템 테스트는 legacy 대비 **변경/일치**를 명시한다.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /st" > log/${TS}_st.md
echo "" >> log/${TS}_st.md
echo "## 사용 프롬프트" >> log/${TS}_st.md
echo "\`/st\`" >> log/${TS}_st.md
```

## 1. 입력 (이외 자료 보지 말 것)

- `legacy/test/system/` — 기준선 시스템 테스트 (인계 대상)
- `test/simulator/` — 수정된 테스트 하니스
- `docs/srs/SRS.md` — 수정된 UC (시나리오 단위)
- `docs/maintenance/impact.md` — ST 계층 변경 스코프

`reference/`(PDF·CICD·objective.md), `docs/specs/`는 열람 금지.

## 2. 작업 — 인계 후 최소 수정

1. `legacy/test/system/`을 최상위 `test/system/`로 인계한다.
2. 수정된 UC·하니스에 맞춰 영향받는 시나리오만 고친다:
   - 회피 시나리오: 우측 센서 자극 세팅·우회전 기대 결과 제거. "전방 막힘 + 좌측 가능 → 좌회전", "전방·좌측 막힘 → 후진"으로 분기 재구성.
   - 우측 센서 자극을 주입하던 setup 코드 제거.
3. 우측 센서와 무관한 시나리오(전원 생명주기, 청소 세션, 강화 청소 등)는 legacy 그대로 둔다. 시나리오 이름·구조는 수정 SRS UC를 따른다(=legacy 구조).

## 3. 실행 검증 (WSL)

```bash
wsl bash -lc "cd '$(pwd)' && cmake --build build -j --target oop_system_test && ctest --test-dir build -R system_tests --output-on-failure"
```

전부 통과해야 완료.

## 4. 차이 표기 (필수)

| 시나리오/파일 | legacy 대비 | 내용 |
|---|---|---|
| 장애물 회피 시나리오 | 변경 | 우회전 경로 케이스 삭제, 좌/후진 분기로 재구성 |
| 전원 생명주기 시나리오 | 일치 | 센서 무관, legacy 그대로 |
| ... | ... | ... |

`wsl bash -lc "diff -rq legacy/test/system test/system"`로 확인.

## 5. 자기 점검

- [ ] 시나리오 구조가 수정 SRS UC(=legacy 구조)와 일치하는가?
- [ ] 우측 센서 자극·우회전 기대값이 일관되게 제거됐는가?
- [ ] 센서 무관 시나리오를 "일치"로 명시했는가? (UC 커버리지 유지)
- [ ] 전부 통과하는가?
- [ ] PDF·CICD·objective.md, docs/specs를 열지 않았는가?

## 6. 로그 종료

`log/${TS}_st.md`에 채움:
- `## 주요 결정`: 회피 시나리오 재구성 방식, 보존한 시나리오
- `## 생성·수정 파일`: 변경된 시스템 테스트 파일
- `## 차이 표기`: legacy 대비 변경 N건 / 일치 M건
- `## 결과 요약`: 시나리오 수, UC 커버리지, 통과율
- `## 검증 방법`: 위 ctest 명령
- `## 다음 단계`: `/sa`

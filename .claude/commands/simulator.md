---
description: legacy/test/simulator를 인계받아 오른쪽 센서 삭제를 반영한 시뮬레이터를 수정한다. legacy 대비 변경/일치를 명시한다.
---

# /simulator — 시뮬레이터 유지보수 (오른쪽 센서 삭제 반영)

> **정책 (CLAUDE.md 2·6·7절)**
> 시뮬레이터를 새로 설계하지 않는다. `legacy/test/simulator`를 인계받아 **우측 센서 삭제가 강제하는 부분만** 수정한다. Sim 클래스 이름·하니스 API는 legacy 그대로.
> PDF·CICD·objective.md는 열람 금지. 수정 시뮬레이터는 legacy 대비 **변경/일치**를 명시한다.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /simulator" > log/${TS}_simulator.md
echo "" >> log/${TS}_simulator.md
echo "## 사용 프롬프트" >> log/${TS}_simulator.md
echo "\`/simulator\`" >> log/${TS}_simulator.md
```

## 1. 입력 (이외 자료 보지 말 것)

- `legacy/test/simulator/` — 기준선 시뮬레이터 + 테스트 하니스 (인계 대상)
- `include/` — 수정된 추상화 인터페이스
- `docs/sdd/SDD.md` — 수정된 설계 (추상화·시퀀스)
- `docs/maintenance/impact.md` — Simulator 계층 변경 스코프

`reference/`(PDF·CICD·objective.md)는 열람 금지.

## 2. 작업 — 인계 후 최소 수정

1. `legacy/test/simulator/`를 최상위 `test/simulator/`로 인계한다.
2. 영향받는 부분만 수정한다:
   - 근접 센서 Sim 구현이 방향 비특정이면 Sim 클래스 자체는 **일치**. 하니스가 "우측 센서 주입 API"를 노출했다면 그 API만 제거.
   - 환경 주입 API에서 우측 센서 자극 세팅 경로 제거 (앞/왼쪽만 남김).
   - 하니스의 시스템 조립부가 우측 센서를 주입했다면 수정 구현에 맞춰 제거.
3. Sim/하니스 이름·구조는 legacy 그대로. 시뮬레이터는 test 전용으로 production(`rvc_core`)에 들어가지 않는다.

## 3. 빌드 검증 (WSL)

```bash
wsl bash -lc "cd '$(pwd)' && cmake --build build -j --target oop_system_test"
```

빌드 성공 시 완료 (실제 시나리오는 `/st`).

## 4. 차이 표기 (필수)

| 시뮬레이터 파일/API | legacy 대비 | 내용 |
|---|---|---|
| `rvc_sim_harness.hpp` | 변경 | 우측 센서 주입 API·조립 제거 |
| `sim_proximity_sensor.hpp` | 일치/변경 | (방향 비특정이면 일치) |
| `sim_dust_sensor.hpp` | 일치 | 센서 무관, legacy 그대로 |
| ... | ... | ... |

`wsl bash -lc "diff -rq legacy/test/simulator test/simulator"`로 확인.

## 5. 자기 점검

- [ ] Sim/하니스 이름·구조를 보존했는가?
- [ ] 우측 센서 주입 경로가 환경 주입 API에서 제거됐는가?
- [ ] 변경 불필요 Sim을 "일치"로 명시했는가?
- [ ] 시스템 테스트 타겟이 빌드되는가?
- [ ] PDF·CICD·objective.md를 열지 않았는가?

## 6. 로그 종료

`log/${TS}_simulator.md`에 채움:
- `## 주요 결정`: 우측 센서 주입 API 제거 방식, 보존한 하니스 구조
- `## 생성·수정 파일`: 변경된 시뮬레이터 파일
- `## 차이 표기`: legacy 대비 변경 N건 / 일치 M건
- `## 결과 요약`: 변경 Sim 수, 제거된 환경 주입 API 수
- `## 검증 방법`: 위 cmake 빌드 명령
- `## 다음 단계`: `/st`

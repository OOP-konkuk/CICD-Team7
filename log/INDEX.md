# Claude 작동 기록 인덱스 (발표 자료)

오른쪽 근접 센서 삭제(센서 3→2) 유지보수의 전 단계 실행 기록이다. 각 로그의 `## 차이 표기`가 발표 슬라이드의 핵심이다.
실행 순서대로 나열한다. 통합 변경/일치 표는 [`docs/maintenance/diff-matrix.md`](../docs/maintenance/diff-matrix.md).

| # | 단계 | 로그 | legacy 대비 차이 (1줄 요약) |
|---|---|---|---|
| 1 | `/change-impact` | [2026-05-29_1349_change-impact.md](2026-05-29_1349_change-impact.md) | 영향 범위 산출: 변경 ~18파일 / 일치 다수 / 검토 4건. 우측 "센서(입력)" 삭제 ≠ 우회전 "모터(출력)" 삭제 구분 |
| 2 | `/srs` | [2026-05-29_1355_srs.md](2026-05-29_1355_srs.md) | SRS 변경: 센서 4→3, 컨텍스트/액터/FR-MOVE-02/UC-03에서 우측 센서 제거·회피 단순화. FR/NFR/UC ID 집합은 일치 |
| 3 | `/sdd` | [2026-05-29_1404_sdd.md](2026-05-29_1404_sdd.md) | SDD 변경: IProximitySensor 3→2 인스턴스, ctor `right_` 제거, 회피 시퀀스·`AvoidPath::TURN_RIGHT` 제거. `MotorCommand::TURN_RIGHT`·`executeBackAndTurn` 일치 |
| 4 | `/code` | [2026-05-29_1404_code.md](2026-05-29_1404_code.md) | 코드 변경 6파일(types/engine h·cpp/handler h·cpp/main). 인터페이스·비센서 컴포넌트·CMakeLists 일치. WSL 클린 빌드 통과 |
| 5 | `/ut` | [2026-05-29_1424_ut.md](2026-05-29_1424_ut.md) | 단위 테스트 변경 2파일(handler·engine), 우측 경로 테스트 삭제·우측 인자 제거. 강도 컨트롤러·stub 일치. ctest 통과 |
| 6 | `/simulator` | [2026-05-29_1434_simulator.md](2026-05-29_1434_simulator.md) | 시뮬레이터 변경 1파일(rvc_sim_harness): 우측 주입/조회 API·배선 제거. sim_proximity_sensor 등 일치 |
| 7 | `/st` | [2026-05-29_1510_st.md](2026-05-29_1510_st.md) | 시스템 테스트 변경 2파일: 우측 경로 시나리오 2건 삭제, 후진 트리거 단순화. 전원/강화/오류 시나리오 일치. 24/24 통과 |
| 8 | `/sa` | [2026-05-29_1518_sa.md](2026-05-29_1518_sa.md) | 정적 분석 재실행: 프로젝트 이슈 0→0 일치, 신규 경고 0. STL 거짓 양성 5→0은 toolchain 차이(legacy도 현재 0) |
| 9 | `/package` | [2026-05-29_1532_package.md](2026-05-29_1532_package.md) | 클린 빌드 재현(53/53 통과, 경고 0) + 통합 diff 매트릭스 + README/INDEX 최종화 |

## 발표 요약 한 문장

> `legacy/`(TP#5 완성본)를 기준선으로, **우측 근접 센서 삭제가 닿는 16개 핵심 파일만 최소 수정**(+ SA 리포트 3 재생성)했고, 나머지(인터페이스·stub·sim·비센서 컴포넌트·CMakeLists)는 **일치**로 명시. 회피 로직은 "좌 가능→좌회전 / 좌 막힘→후진"으로 단순화, 우회전 **모터 명령**은 존속. 클린 빌드 53/53 통과·정적 분석 클린.

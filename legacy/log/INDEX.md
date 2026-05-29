# AI 의사결정 흐름 인덱스

**프로젝트**: RVC Control SW — TeamPractice#5  
**빌드 기준일**: 2026-05-21  
**최종 결과**: 빌드 경고 0, 테스트 56/56 (100%)

---

## 슬래시 커맨드 실행 순서

| # | 커맨드 | 로그 | 핵심 결정 |
|---|--------|------|-----------|
| 1 | `/extract-specs` | [2026-05-21_1414_extract-specs.md](2026-05-21_1414_extract-specs.md) | PDF 9 UC → 6 UC 재증류; 시간 한계를 FR 검증 기준에 내재화; 이전 학기 클래스명 전면 폐기 |
| 2 | `/srs` | [2026-05-21_1422_srs.md](2026-05-21_1422_srs.md) | FR-CTRL/MOVE/CLEAN/SENSE, NFR-TIMING/ARCH/SAFETY/BUILD 4범주 ID 체계 확정 |
| 3 | `/sdd` | [2026-05-21_1439_sdd.md](2026-05-21_1439_sdd.md) | 4계층 아키텍처; CleaningEngine·ObstacleHandler·CleanIntensityController 3 컴포넌트로 단순화 |
| 4 | `/code` | [2026-05-21_1458_code.md](2026-05-21_1458_code.md) | resume() 추가(Boost 재활성); FetchContent GTest 채택; executeTurn() 원자 패턴 |
| 5 | `/ut` | [2026-05-21_1513_ut.md](2026-05-21_1513_ut.md) | 37 테스트; 경계값 5000ms, 누적 틱, 부스트 중 장애물 시나리오 |
| 6 | `/simulator` | [2026-05-21_1542_simulator.md](2026-05-21_1542_simulator.md) | RVCSimHarness 통합 하니스; 이중 모드 주입(setBlocked + enqueueEvent) |
| 7 | `/st` | [2026-05-21_1558_st.md](2026-05-21_1558_st.md) | UC-01~06 100% 커버; 19 시나리오; schedulePause()로 타이밍 제어 |
| 8 | `/sa` | [2026-05-21_1554_sa.md](2026-05-21_1554_sa.md) | front_ 미사용 멤버 제거; C-배열→std::array; GCC -fanalyzer 거짓 양성 5건 억제 |
| 9 | `/package` | [2026-05-21_1629_package.md](2026-05-21_1629_package.md) | 클린 빌드 56/56; # 경로 symlink 해결책 CLAUDE.md 등록; README 최종화 |

---

## 주요 설계 의사결정 흐름

### 1. 요구사항 재증류 (`/extract-specs`)
- PDF의 9개 UC를 6개로 통합 (회피 분기 UC4·5·6 → 단일 UC-03 + 분기 표)
- NFR-P-* 시간 한계를 별도 NFR이 아니라 각 FR의 검증 기준으로 내재화
- 이전 학기 클래스명(RVCOrchestrator, MovementPolicyController 등) 전면 폐기

### 2. 아키텍처 결정 (`/sdd`)
- **PDF 6 컴포넌트 → 3 컴포넌트**: CleaningEngine(중앙 조율) + ObstacleHandler(회피 전략) + CleanIntensityController(청소 강도)
- **ObstacleHandler에서 front_ 제거**: 전방 감지는 CleaningEngine이, 좌우 회피 결정은 ObstacleHandler가 담당 (단일 책임 원칙)
- **6개 순수 추상 인터페이스**: IProximitySensor, IDustSensor, IDriveTrain, ICleaningUnit, IOutputLog, ICommandSource

### 3. 테스트 가능성 (`/ut`, `/simulator`)
- 단위 테스트: Stub으로 격리 (test/unit/stub/)
- 시스템 테스트: Sim 구현 + RVCSimHarness (test/simulator/)
- 이중 주입 패턴: `setBlocked(bool)` (지속 상태) + `enqueueEvent(bool)` (단발 주입)

### 4. 빌드 환경 (#경로 문제)
- 프로젝트 경로에 `#5` 포함 → GNU Make `$(CMAKE_SOURCE_DIR)` 변수 값이 `#`에서 잘림
- 해결: `/tmp/rvc_proj` symlink → cmake 소스/빌드 경로를 `#` 없는 경로로 우회

---

## 재현 명령 (한 줄)

```bash
wsl bash -lc "ln -sfn '/mnt/c/Users/윤성진/OneDrive/바탕 화면/Konkuk/3-1/객체지향개발방법론/과제/TeamPractice#5/dev/#5_Team7' /tmp/rvc_proj && cmake -S /tmp/rvc_proj -B /tmp/rvc_build && cmake --build /tmp/rvc_build -j && ctest --test-dir /tmp/rvc_build --output-on-failure"
```

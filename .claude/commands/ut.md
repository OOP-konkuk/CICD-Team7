---
description: 구현된 컴포넌트 단위 테스트(GTest)를 test/unit/에 작성하고 ctest를 실행한다. 테스트 단위는 실제로 구현된 클래스 구조를 따른다.
---

# /ut — Unit Tests

> **정책**
> 테스트 파일과 단위는 **실제 `include/`·`src/`에 존재하는 클래스**에 맞춰 도출한다.
> PDF Domain Model의 클래스 분해를 기준으로 테스트 파일 이름을 미리 정해두지 않는다.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /ut" > log/${TS}_ut.md
echo "" >> log/${TS}_ut.md
echo "## 사용 프롬프트" >> log/${TS}_ut.md
echo "\`/ut\`" >> log/${TS}_ut.md
```

## 1. 입력

- `include/`, `src/` (현재 구현 — 테스트 단위의 진실 소스)
- `docs/sdd/SDD.md` (10절 테스트 가능성 보장·Stub 격리 전략, 11절 추적성 매트릭스)
- `docs/srs/SRS.md` (UC 사전/사후 조건, FR 검증 기준)

`reference/`, `docs/specs/`는 본 단계에서 열지 않는다.

## 2. 테스트 단위 도출

1. `include/`·`src/`를 훑어 **실제로 존재하는 테스트 가능한 클래스**(단일 책임을 가진 컴포넌트, 상태나 결정 로직을 보유한 것)를 식별.
2. 클래스마다 `test/unit/<ClassName>Test.cpp` 1개를 두는 것을 기본으로 한다. 파일명은 **실제 클래스명을 따른다** — PDF 이름을 가정해 미리 적어두지 않는다.
3. 외부 의존성(센서·모터 등)을 받는 컴포넌트는 Stub/Spy로 격리해 테스트.

## 3. 산출물

`test/unit/` 아래 GTest 기반 테스트 파일. 구조:

- 컴포넌트별 테스트 파일 (이름은 실제 클래스에 맞춰 결정)
- Stub/Spy 클래스는 `test/unit/stubs/` 또는 각 테스트 파일 내부에 두되, **production 코드(`src/`, `include/`)에 절대 두지 않는다**.

## 4. 테스트 작성 규칙

- 각 컴포넌트 공개 메서드마다 **Positive + Negative** 최소 1쌍.
- 테스트 이름: `<Subject>_<Scenario>_<ExpectedOutcome>` (예: `<클래스>_<상황>_<기대결과>`).
- 출력 검증은 SDD가 정한 출력 통제점에 대한 의존성 주입을 통해 수행. `testing::internal::CaptureStdout()`은 최후의 수단.
- 시간 의존 코드는 의존성 주입으로 격리.
- 한 테스트가 한 가지만 검증. assertion 폭주 금지.
- 각 테스트 파일 상단 주석에 검증 대상 SRS FR/UC ID 명시 (SDD 추적성 매트릭스 활용).

## 5. 빌드·실행 검증 (WSL)

```bash
wsl bash -lc "cd '$(pwd)' && cmake --build build -j --target oop_test && ctest --test-dir build -R unit_tests --output-on-failure"
```

모든 테스트 통과 + 빌드 경고 0 일 때만 본 단계 완료.

커버리지 측정(권장, SRS 유지보수성 NFR 충족 보조):
```bash
wsl bash -lc "cd '$(pwd)' && cmake -S . -B build-cov -DCMAKE_CXX_FLAGS='--coverage -O0 -g' && cmake --build build-cov -j && ctest --test-dir build-cov && lcov --capture --directory build-cov --output-file coverage.info --no-external && lcov --list coverage.info"
```

## 6. 자기 점검

- [ ] 테스트 파일명이 실제 `include/`·`src/`의 클래스명과 일치하는가?
- [ ] PDF Domain Model 클래스명(RVCPowerControllerTest, MotorControllerTest 등)을 실제 클래스가 존재하지 않는데도 사용하지 않았는가?
- [ ] 모든 테스트 가능한 컴포넌트에 대해 테스트 파일이 존재하는가?
- [ ] SDD 추적성 매트릭스의 모든 FR이 어떤 테스트에서 검증되는가?
- [ ] Production 코드에 Stub/Mock가 섞이지 않았는가?

## 7. 로그 종료

`log/${TS}_ut.md`에 채움:
- `## 주요 결정`: Stub vs Mock 선택, FR ↔ 테스트 매핑, 커버리지 목표
- `## 생성·수정 파일`: 추가된 테스트 파일 목록
- `## 결과 요약`: 테스트 개수, 통과율, 커버리지 (가능 시)
- `## 검증 방법`: 위 ctest 명령
- `## 다음 단계`: `/simulator`

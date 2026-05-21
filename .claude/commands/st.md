---
description: SRS의 유스케이스를 시나리오로 묶어 시스템 테스트(GTest)를 test/system/에 작성하고 실행한다. /simulator의 테스트 하니스를 사용.
---

# /st — System Tests

> **정책**
> 시나리오는 **SRS의 UC**를 단위로 도출한다.
> 시나리오 그룹·이름은 SRS의 UC ID·이름을 따라 자연스럽게 — PDF UC 번호(UC1~9)나 PDF가 제안한 분류를 가정하지 않는다.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /st" > log/${TS}_st.md
echo "" >> log/${TS}_st.md
echo "## 사용 프롬프트" >> log/${TS}_st.md
echo "\`/st\`" >> log/${TS}_st.md
```

## 1. 입력

- `test/simulator/` 의 테스트 하니스 (이름은 `/simulator` 단계에서 결정)
- `docs/srs/SRS.md` (UC 시나리오 — 시스템 테스트의 단위)
- `include/`, `src/` (검증 대상)

`reference/`, `docs/specs/`는 본 단계에서 열지 않는다.

## 2. 시나리오 도출

1. SRS의 UC 목록에서 각 UC를 1개 이상의 시나리오로 분해.
2. 시나리오 단위는 SRS UC ID 또는 의미 있는 이름으로 (예: 전원 생명주기, 청소 세션, 장애물 회피, 강화 청소, 오류 처리 — 명칭은 자유).
3. 각 시나리오는 기본 흐름 + 대안/예외 흐름을 모두 다루도록 분기.

## 3. 산출물

`test/system/` 아래 GTest 파일 (1개 통합 또는 시나리오 그룹별로 분리):

- 각 시나리오는 다음 패턴:
  1. 테스트 하니스 초기화
  2. 환경 주입 API로 외부 자극 세팅
  3. 사용자 입력 시퀀스 전달 (예: `sim.input("power on")` — 정확한 API는 `/simulator`가 정의)
  4. 상태 쿼리 API로 결과 검증

## 4. 작성 규칙

- 테스트당 한 시나리오만. Assertion은 시나리오 후반에 집중.
- 시나리오 시작 부분 주석에 검증 대상 SRS UC/FR ID 명시.
- 시간 관련 검증은 SRS NFR의 시간 한계와 일치해야 함.
- 시나리오는 SRS UC의 **기본 흐름 + 대안/예외 흐름**을 모두 다루도록 분기.

## 5. XML 리포트 (선택)

CTest가 XML 리포트를 생성하도록:

```cmake
add_test(NAME system_tests COMMAND oop_system_test --gtest_output=xml:system_tests.xml)
```

## 6. 실행 검증 (WSL)

```bash
wsl bash -lc "cd '$(pwd)' && cmake --build build -j --target oop_system_test && ctest --test-dir build -R system_tests --output-on-failure"
```

전부 통과해야 본 단계 완료.

## 7. 자기 점검

- [ ] SRS의 모든 UC가 어떤 시나리오에서 검증되는가? (UC 커버리지 100%)
- [ ] 각 UC의 기본 흐름 + 대안/예외 흐름이 모두 시나리오로 분기되어 있는가?
- [ ] 시나리오 이름이 SRS UC 이름과 추적 가능한가? (PDF UC1~9 번호를 가정하지 않았는가?)
- [ ] 모든 시간 관련 검증이 SRS NFR과 일치하는가?

## 8. 로그 종료

`log/${TS}_st.md`에 채움:
- `## 주요 결정`: 시나리오 그룹화 기준, 사용자 입력 시퀀스 추상화 방식
- `## 생성·수정 파일`: 추가된 시스템 테스트 파일
- `## 결과 요약`: 시나리오 수, UC 커버리지, 통과율
- `## 검증 방법`: 위 ctest 명령
- `## 다음 단계`: `/sa`

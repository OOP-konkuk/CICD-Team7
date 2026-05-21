---
description: SDD(Software Design Document)를 docs/sdd/SDD.md에 작성한다. 입력은 docs/srs/SRS.md만. 클래스 구조·이름·계층은 Claude가 SRS 위에서 처음부터 도출한다.
---

# /sdd — Software Design Document 작성

> **정책 (CLAUDE.md 2·6절)**
> 본 단계가 RVC의 **설계(How)**를 처음 결정한다.
> PDF·이전 코드의 Domain Model·Sequence Diagram·메서드 시그니처를 답습하지 않는다. 클래스 분해·인터페이스 추상화·계층 구조는 SRS의 요구사항으로부터 Claude가 직접 도출한다.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /sdd" > log/${TS}_sdd.md
echo "" >> log/${TS}_sdd.md
echo "## 사용 프롬프트" >> log/${TS}_sdd.md
echo "\`/sdd\`" >> log/${TS}_sdd.md
```

## 1. 입력 (이외 자료 보지 말 것)

- `docs/srs/SRS.md` (주 입력)

`docs/specs/`, `reference/` 폴더는 모두 다시 열지 않는다. SRS가 단일 진실 소스이다.

## 2. 산출물

`docs/sdd/SDD.md` — 필수 섹션 (내용은 Claude가 SRS에서 도출):

1. **설계 개요**
   - 설계 목표 (SRS NFR 충족 기준)
   - 설계 원칙 (SOLID, 의존성 역전, 테스트 용이성, 단일 책임)
2. **아키텍처 결정**
   - 계층 구조를 Claude가 명명하고 그린다 (mermaid). 계층 수·이름·역할은 SRS 요구사항에서 직접 도출 — PDF Domain Model의 6 컨트롤러 분해를 강제 채택하지 않는다.
   - 결정 근거: 왜 이 분해인가, 어떤 NFR을 충족시키는가
3. **컴포넌트 설계**
   - 각 컴포넌트(클래스/모듈)마다: 책임(한 문장), 의존하는 추상화, 공개 메서드 시그니처(C++17), 상태 관리 방식
   - 컴포넌트 수와 명칭은 Claude가 결정 — PDF의 `RVCOrchestrator`·`MotorController`·`MovementPolicyController` 같은 이름을 그대로 채택해서는 안 된다 (의도적으로 다른 이름·다른 분해 권장)
4. **추상화 인터페이스**
   - 외부 환경(센서·구동기·사용자 입출력 등)을 추상화하는 인터페이스 명세
   - 인터페이스명·메서드명 모두 Claude가 결정 — PDF Sequence Diagram의 `startCleaning()`·`requestStatus()` 등 메시지명을 그대로 받아쓰지 않는다
5. **상태·이벤트 모델**
   - 시스템 운영 모드와 전이를 다이어그램으로 표현 (mermaid stateDiagram)
   - SRS의 UC 흐름과 일치해야 함
6. **시퀀스 설계** (mermaid sequenceDiagram)
   - SRS UC 중 핵심 3~5개에 대한 시퀀스
   - 등장 메시지명은 Claude가 SRS 요구에서 새로 정의 (PDF 다이어그램 메시지 그대로 재현 금지)
7. **데이터·타입 설계**
   - enum, 값 타입, 공용 타입 헤더 위치
   - 타입 이름은 Claude 결정 — `DirectionType`·`StateType`·`ErrorType` 같은 PDF 이름의 답습을 피한다
8. **디렉토리·파일 매핑**
   - 어떤 컴포넌트가 어느 `include/<sub>/`, `src/<sub>/` 파일에 들어갈지 표
   - 디렉토리 구조도 Claude 결정 — `controller/`·`handler/`·`hardware/` 같은 분류가 자연스럽다면 채택하되, 더 적합한 구조가 있으면 그것을 택한다
9. **빌드 설계**
   - CMake 타겟 정의 (메인 실행파일 + 단위 테스트 실행파일 + 시스템 테스트 실행파일)
   - 의존성 그래프 (rvc_core 라이브러리 + 실행파일 분리 권장, 단 Claude가 더 나은 구조를 도출하면 그것 채택)
10. **테스트 가능성 보장**
    - 모든 컴포넌트를 Stub으로 격리해 단위 테스트 가능함을 설계 차원에서 보장 (SRS NFR 충족)
    - 출력 사이드 이펙트(`std::cout` 등)는 단일 책임 통제점을 거쳐야 함
11. **SRS ↔ 컴포넌트 추적성 매트릭스**
    - SRS의 FR-ID ↔ Claude가 새로 정의한 컴포넌트/메서드 매핑

## 3. 작성 규칙

- 시그니처는 C++ 코드블록으로 정확히 적는다 (`/code` 단계가 그대로 받아쓸 수 있도록).
- 의존성은 **구현 → 추상화** 방향(의존성 역전).
- 모든 컴포넌트는 단위 테스트가 가능해야 함을 설계에서 보장.
- 클래스명·인터페이스명·메서드명을 짓기 전에 "PDF에서 같은 이름이 있었나? 있다면 의도적으로 다르게 짓는 것이 더 명료한가?" 자문한다.

## 4. 자기 점검

- [ ] PDF Domain Model의 클래스명(RVCOrchestrator, MovementPolicyController, CleaningPolicyController, RVCPowerController, MotorController, CleanerController, ErrorHandler 등)이 SDD에 그대로 등장하지 않는가? (의도적으로 다른 이름을 선택했는가?)
- [ ] PDF Sequence Diagram의 메시지명(`startCleaning()`, `requestStatus()`, `moveForward()`, `powerUp()`, `stopMoving()`, `stopCleaning()`, `shutdown()`, `turnLeft()`, `turnRight()`, `moveBackward()` 등)이 SDD 인터페이스 시그니처로 그대로 등장하지 않는가?
- [ ] 모든 SRS FR이 추적성 매트릭스에서 어떤 컴포넌트에 매핑되어 있는가?
- [ ] 모든 SRS NFR이 설계의 어느 측면에서 충족되는지 명시되어 있는가?
- [ ] `reference/`·`docs/specs/`를 본 단계에서 한 번도 읽지 않았는가?

## 5. 로그 종료

`log/${TS}_sdd.md`에 채움:
- `## 주요 결정`:
  - 계층 구조·분해 단위 결정 근거
  - PDF Domain Model과 의도적으로 다르게 결정한 항목 (이름 변경, 컴포넌트 통합/분리, 의존성 방향 변경 등)과 그 근거 — **이 섹션이 발표 자료의 핵심**
  - Stub 격리 방식, 출력 사이드 이펙트 통제 지점
- `## 생성·수정 파일`: `docs/sdd/SDD.md`
- `## 결과 요약`: 컴포넌트 수, 추상화 인터페이스 수, mermaid 다이어그램 수, PDF 대비 분해 차이 요약
- `## 검증 방법`: `wsl bash -lc "wc -l docs/sdd/SDD.md && grep -c '\`\`\`mermaid' docs/sdd/SDD.md"`
- `## 다음 단계`: `/code`

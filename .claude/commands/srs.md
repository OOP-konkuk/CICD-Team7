---
description: SRS(Software Requirements Specification)를 docs/srs/SRS.md에 작성한다. 입력은 docs/specs/(재증류 결과)만. reference/는 다시 열지 않는다.
---

# /srs — Software Requirements Specification 작성

> **정책 (CLAUDE.md 2·6절)**
> 본 단계는 `/extract-specs`가 만든 **재증류된 명세**를 SRS 형식으로 재구성하는 단계다.
> PDF·이전 코드·원본 요구사항 ID는 다시 보지 않는다. 클래스 구조·메서드명은 SRS에서 결정하지 않는다 (그건 `/sdd`의 몫).

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /srs" > log/${TS}_srs.md
echo "" >> log/${TS}_srs.md
echo "## 사용 프롬프트" >> log/${TS}_srs.md
echo "\`/srs\`" >> log/${TS}_srs.md
```

## 1. 입력 (이외 자료 보지 말 것)

- `docs/specs/system-overview.md`
- `docs/specs/requirements.md` (재증류된 FR/NFR + 추적성 표)
- `docs/specs/use-cases.md` (재증류된 UC)

`reference/` 폴더는 절대 다시 열지 않는다. 원본 PDF ID(R1.1, NFR-O-06 등)나 PDF 클래스명(RVCOrchestrator 등)은 본 단계의 입력이 아니다.

## 2. 산출물

`docs/srs/SRS.md` — 다음 섹션 구조 필수:

1. 문서 개요 (목적, 범위, 정의·약어, 참고 문서 — `docs/specs/`만 명시)
2. 시스템 개요
   - 시스템 컨텍스트 (mermaid context diagram — `system-overview.md`를 SRS 형식으로 재배치)
   - 외부 액터 (재증류된 명세에 정의된 액터만)
3. 기능 요구사항 (FR)
   - `docs/specs/requirements.md`의 FR을 SRS 표 형식으로 옮긴다 (ID 유지)
   - 각 FR은 검증 가능한 진술 + 측정 기준 포함
4. 비기능 요구사항 (NFR)
   - `docs/specs/requirements.md`의 NFR을 카테고리별로 재배치 (성능·아키텍처·빌드·인터페이스 등)
5. 유스케이스 (UC)
   - `docs/specs/use-cases.md`의 UC를 SRS 표준 양식(ID/이름/액터/사전조건/주요 흐름/대안·예외/사후조건)으로 재배치
6. 외부 인터페이스
   - 사용자 인터페이스: 재증류된 명세의 CLI 요구만
   - 하드웨어 추상화: "외부 환경과의 경계가 추상화되어야 한다"는 수준의 요구 (구체 인터페이스 이름·시그니처는 적지 않음 — `/sdd`가 결정)
7. 추적성 매트릭스
   - 재증류된 FR ↔ UC 매핑 (다음 단계 `/sdd`가 "컴포넌트" 열을 채우도록 빈 칸 유지)

## 3. 작성 규칙

- **재증류된 새 ID만 사용**한다. 원본 PDF ID(R*, NFR-O-*, UC1~9)는 본문 어디에도 등장하면 안 된다.
- "구현 방법"을 적지 않는다 — "무엇을, 왜"만 적는다. 클래스명·메서드 시그니처·계층 구조는 본 단계에서 결정하지 않는다.
- 모든 표/다이어그램은 마크다운/mermaid로 인라인.
- mermaid는 **context diagram 수준만**. 도메인 클래스 다이어그램은 `/sdd`에서 그린다.
- 외부 의존성을 새로 도입할 만한 요구는 명시적으로 표시.

## 4. 자기 점검

- [ ] PDF 원본 ID(R*, NFR-O-* 등)가 SRS 본문에 등장하지 않는가?
- [ ] PDF Domain Model 클래스명(RVCOrchestrator, MovementPolicyController 등)이 SRS 본문에 등장하지 않는가?
- [ ] FR/NFR/UC가 모두 `docs/specs/`의 ID 체계와 일치하는가?
- [ ] `reference/` 폴더를 본 단계에서 한 번도 읽지 않았는가?

## 5. 로그 종료

`log/${TS}_srs.md`에 다음 채움:
- `## 주요 결정`: 재증류된 명세를 SRS 형식으로 어떻게 재배치했는지, 사후조건·검증 기준을 어떻게 명료화했는지
- `## 생성·수정 파일`: `docs/srs/SRS.md` 절대 경로
- `## 결과 요약`: FR 개수, NFR 개수, UC 개수, 추적성 표 행 수
- `## 검증 방법`: `wsl bash -lc "wc -l docs/srs/SRS.md && grep -cE '^### (FR|NFR|UC)-' docs/srs/SRS.md"`
- `## 다음 단계`: `/sdd`

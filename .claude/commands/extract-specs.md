---
description: reference/ (PDF 4개 + objective.md + CICD-Team7-main 코드)를 *이해*하여, RVC 시스템의 요구사항(FR/NFR/UC)을 Claude의 언어로 재증류하여 docs/specs/에 산출한다. 원본 텍스트·다이어그램·시그니처를 그대로 옮기지 않는다.
---

# /extract-specs — 명세 재증류

> **정책 (CLAUDE.md 2·6절 재확인)**
> 본 단계의 목표는 "원본을 마크다운으로 옮기는 변환"이 아니라, **시스템의 의도를 이해하고 더 명료한 형태로 재진술**하는 것이다.
> 따라서 결과 문서는 PDF와 ID·구조·표현이 다르고, 더 짧고 명확해야 정상이다. PDF 텍스트를 그대로 복사하는 것은 본 단계의 실패이다.

## 0. 로그 시작 (반드시 가장 먼저)

WSL bash로 타임스탬프 변수를 잡고 빈 로그 파일을 만든다.

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /extract-specs" > log/${TS}_extract-specs.md
echo "" >> log/${TS}_extract-specs.md
echo "## 사용 프롬프트" >> log/${TS}_extract-specs.md
echo "\`/extract-specs\`" >> log/${TS}_extract-specs.md
```

이후 진행하면서 `log/${TS}_extract-specs.md`에 섹션을 채워 나간다.
모든 작업 종료 시 다음 섹션이 채워져 있어야 한다: `## 주요 결정`, `## 생성·수정 파일`, `## 결과 요약`, `## 검증 방법`, `## 다음 단계`.

## 1. 본 작업 — 3단계 재증류

### A. 1차 이해 (Reference 자료 통독 — 산출물 없음)

`reference/`의 모든 1차 자료를 **이해 목적으로** 읽는다. 이 단계는 산출물 파일을 만들지 않으며, Claude의 작업 컨텍스트에 도메인 모델을 형성하는 데 사용된다.

읽기 대상:
- `reference/객체지향개발방법론#1_V3.pdf`, `#2_V2.pdf`, `#3_v2.pdf`, `#4.pdf` — 팀의 명세
- `reference/objective.md` — 본 과제 목표
- `reference/CICD-Team7-main/` — 이전 학기 7팀의 코드 (디렉토리·헤더 위주, 함수 본문은 깊이 읽지 않음)

읽기 목적 (Claude가 답할 수 있어야 함):
1. 이 시스템의 **존재 이유**는 무엇인가? (한 줄)
2. 외부에서 시스템에 **어떤 자극**이 들어오는가? (액터·이벤트)
3. 시스템은 **어떤 상태들**을 가지는가? (개념적 상태, PDF 클래스명 무관)
4. **품질·환경 조건**(시간 한계, 추상화 요구, 빌드 환경 등)은 무엇인가?
5. **본질적으로 중복·과세분화된 요구사항**은 어떤 것인가? (재증류 후보)

대상 자료의 다이어그램·클래스명·메서드명·테스트 케이스명은 **읽되 기억의 닻으로만 쓴다.** 그대로 옮길 대상이 아니다.

### B. 재증류 산출 (`docs/specs/`)

읽기를 마치면 다음 3개 문서만 만든다. 원본 PDF별 1:1 변환 문서(`01-team-project-1.md` 등)는 **만들지 않는다**.

#### B-1. `docs/specs/system-overview.md`

Claude가 작성하는 1~2페이지짜리 시스템 개요. 형식 자유, 다만 다음을 포함:

- **시스템 정의**: 한 단락(3~5줄)으로 RVC가 무엇이고 왜 필요한가를 자기 언어로 서술
- **외부 환경 모델**: 어떤 액터·센서·물리적 환경이 입력으로 들어오고 시스템이 무엇을 출력하는가 (mermaid context diagram 1개)
- **본질 동작 모드**: 시스템이 가지는 운영 모드를 Claude가 정리 (예: 정상 청소, 회피, 강화 청소, 종료 — 명칭은 자유)
- **품질 핵심**: 본 시스템 설계에서 가장 강한 제약이 되는 NFR 3~5개를 골라 강조

PDF의 Use Case Diagram, Domain Model을 **mermaid로 재현하지 않는다.** context diagram은 외부 액터-시스템 경계 수준만.

#### B-2. `docs/specs/requirements.md` — 재증류된 FR/NFR

원본의 R1.1~R6.1, NFR-P/O/OE/I를 **Claude가 새로 정의한 ID 체계로 재진술**한다. 권장 접근:

- **새 ID 체계** (예시일 뿐, Claude가 자율 결정):
  - 기능: `FR-<도메인>-<번호>` (예: `FR-MOTION-01`, `FR-SENSE-02`, `FR-POWER-01`)
  - 비기능: `NFR-<범주>-<번호>` (예: `NFR-TIMING-01`, `NFR-ARCH-01`, `NFR-BUILD-01`)
  - 유스케이스: `UC-<번호>` 또는 의미 있는 이름
- **재구성 규칙**:
  - 중복·인접한 요구사항은 통합 (예: R3.3·R3.4·R3.5 의 회피 분기 3개를 하나의 FR로 통합하고, 분기는 본문 표로 표현)
  - 묵시적 가정은 명시화 (예: "회전 중 센서 무시"가 NFR-O-06으로 분리되어 있다면, 해당 동작의 FR에 직접 명시화하는 것이 더 명료한지 판단)
  - 시간 제약(NFR-P-*)은 해당 FR에 측정 가능한 검증 기준으로 부속시키는 안도 고려
- **필수 보존 사항** (변형 가능하지만 누락 금지):
  - C++17 + CMake≥3.14 (원본 NFR-OE-01)
  - 추상 클래스 기반 분리·테스트 용이성 (원본 NFR-O-01·O-02)
  - 시간 한계 수치(50ms, 100ms, 500ms, 5초 등)
  - 장애물 감지 > 먼지 감지 우선 규칙 (원본 NFR-O-09)
  - 회전·터보 진행 중 입력 무시 규칙 (원본 NFR-O-06·O-07)
  - 단, 터보 중에도 장애물 회피는 실행 (원본 NFR-O-08)
- **금지**: PDF 표 셀의 한국어 문장을 그대로 복사. 모든 항목은 Claude가 자기 언어로 다시 쓴다.

문서 끝에 **추적성 표**를 둔다:

| 원본 ID (PDF) | 새 ID | 흡수 방식 |
|---|---|---|
| R1.1 | FR-POWER-01 | 그대로 흡수 |
| R3.3, R3.4, R3.5 | FR-MOTION-03 | 통합 (분기는 표로) |
| NFR-O-06 | FR-MOTION-04 의 부속 규칙 | FR에 명시화 |
| ... | ... | ... |

이 표가 표절이 아닌 재해석임을 입증한다.

#### B-3. `docs/specs/use-cases.md` — 재증류된 UC

원본 UC1~UC9의 Use Case Details를 **본질 흐름만 남기고 재진술**한다. 권장 접근:

- 한 UC는 다음 5요소로 정리: **트리거**, **사전 상태**, **주요 흐름**, **분기/예외**, **검증 시그널** (시스템 테스트가 무엇으로 통과를 판단할지)
- PDF의 "Typical/Alternative/Exceptional Courses of Events" 텍스트와 Sequence Diagram 메시지명(`startCleaning()`, `requestStatus()` 등)은 **그대로 옮기지 않는다.** Claude가 본질만 추출.
- UC 개수도 자유 — 9개를 그대로 유지할지, 통합할지(예: UC4·UC5·UC6 Avoid 계열을 1개 UC로 통합하고 분기 표로 표현)는 Claude가 판단
- 각 UC 끝에 "관련 FR" 목록 (B-2의 새 ID 사용)

### C. 정책 명시 (산출물 말미)

`docs/specs/requirements.md`와 `docs/specs/use-cases.md` 각 파일 끝에 다음 문단을 명시:

> 본 문서는 `reference/`의 PDF·`objective.md`·`CICD-Team7-main/`을 Claude가 통독·이해한 결과를 **재증류**하여 작성한 것이며, 원본 텍스트를 그대로 옮긴 것이 아니다.
> 이후 모든 산출물(`/srs` ~ `/package`)은 `docs/specs/`의 본 문서만 입력으로 사용하고, `reference/` 자료는 다시 참조하지 않는다.

## 2. 로그 종료

위에서 만든 `log/${TS}_extract-specs.md`에 다음 섹션을 채워 저장한다:

- `## 주요 결정`:
  - 어떤 원본 요구사항들을 어떻게 통합·분리·재진술했는지 핵심 사례 3~5개
  - 의도적으로 형식을 바꾼 부분(예: NFR-P-* 시간 한계를 FR 부속 검증 기준으로 옮긴 경우)과 그 근거
  - PDF Domain Model·Sequence Diagram을 추출 대상에서 제외한 사실 (CLAUDE.md 2·6절)
- `## 생성·수정 파일`: `docs/specs/system-overview.md`, `requirements.md`, `use-cases.md` 의 절대 경로
- `## 결과 요약`: 재증류된 FR 개수, NFR 개수, UC 개수, 원본 대비 통합/분리 건수, 보존된 강제 조항 체크
- `## 검증 방법`: `wsl bash -lc "ls docs/specs && wc -l docs/specs/*.md && grep -c '^- ' docs/specs/requirements.md"`
- `## 다음 단계`: `/srs`

## 3. 출력 규약

- 모든 문서는 UTF-8, 한국어, GitHub Flavored Markdown.
- mermaid는 **외부 액터-시스템 경계 수준의 context diagram**만 허용. 내부 클래스 다이어그램·시퀀스 다이어그램은 본 단계에서 그리지 않는다 (그건 `/sdd`의 몫).
- 원본 ID 인용 시 추적성 표 안에서만 사용. 본문 서술에는 새 ID만 등장하도록 한다.
- 절대 경로 인용 시 `reference/`로 시작하는 상대 경로 사용.

## 4. 자기 점검 (산출물 작성 후 Claude가 스스로 확인)

다음 질문에 모두 "예"라고 답할 수 있어야 본 단계가 완료된 것이다:

- [ ] 원본 PDF의 표·문장을 한 줄이라도 그대로 복사하지 않았는가?
- [ ] 원본의 강제 조항(C++17/CMake3.14, 시간 한계, 추상화·Stub 요구, 우선순위 규칙)을 모두 새 사양에서 추적 가능한가?
- [ ] 추적성 표로 원본 ID와 새 ID의 매핑이 모두 확인 가능한가?
- [ ] PDF의 클래스명(RVCOrchestrator 등)·메서드명(`startCleaning()` 등)·CICD-Team7 디렉토리명이 산출물에 등장하지 않는가?
- [ ] 새 UC의 흐름이 원본보다 더 짧고 검증 가능한가?

자기 점검 결과를 로그의 `## 결과 요약`에 짧게 기록한다.

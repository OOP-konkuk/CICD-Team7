---
description: new_objective.md(오른쪽 센서 삭제)와 legacy/ 기준선을 읽어, 변경이 닿는 아티팩트·파일·ID와 닿지 않는(일치 예정) 항목을 docs/maintenance/impact.md로 산출한다. 이후 모든 유지보수 단계의 스코프 기준.
---

# /change-impact — 변경 영향 분석 (유지보수 진입점)

> **정책 (CLAUDE.md 2·6절)**
> 본 단계는 유지보수의 첫 단계로, "오른쪽 센서 삭제"가 `legacy/`의 어디에 닿는지를 식별한다.
> 입력은 **`legacy/` + `reference/new_objective.md`뿐**이다. PDF·`CICD-Team7-main/`·`objective.md`(원본 출처)는 **절대 열지 않는다**. 재설계가 아니라 영향 범위 식별이 목표다.

## 0. 로그 시작 (반드시 가장 먼저)

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /change-impact" > log/${TS}_change-impact.md
echo "" >> log/${TS}_change-impact.md
echo "## 사용 프롬프트" >> log/${TS}_change-impact.md
echo "\`/change-impact\`" >> log/${TS}_change-impact.md
```

## 1. 입력 (이외 자료 보지 말 것)

- `reference/new_objective.md` — 변경 명세 (오른쪽 근접 센서 삭제, 3→2)
- `legacy/` — TP#5 완성 산출물 전체 (docs/, include/, src/, test/, CMakeLists.txt, README.md)

`reference/`의 PDF·`CICD-Team7-main/`·`objective.md`는 **열람 금지**.

## 2. 본 작업 — 영향 추적

`legacy/` 전체에서 오른쪽 센서가 흐르는 경로를 추적한다. 권장 절차:

1. `legacy/`에서 오른쪽 센서 관련 토큰을 검색한다 (예: `right`, `우측`, `오른쪽`, 근접 센서 추상화의 사용처).
2. 각 히트를 **아티팩트 계층별**(명세 → SRS → SDD → Code → UT → Simulator → ST → SA → README)로 분류한다.
3. 각 파일을 다음 셋 중 하나로 판정한다:
   - **변경(MODIFY)**: 오른쪽 센서 삭제로 내용이 바뀌어야 함
   - **일치(KEEP)**: 오른쪽 센서와 무관하거나 방향 비특정이라 legacy 그대로 유지
   - **검토(REVIEW)**: 영향 여부가 애매해 해당 단계 커맨드에서 재확인 필요
4. 의미적 변화도 적는다: 우측 센서 제거 시 회피 경로 선택 로직이 "좌측 가능 → 좌회전, 아니면 후진"으로 단순화되는지 등(legacy 코드의 실제 분기를 근거로).

## 3. 산출물 — `docs/maintenance/impact.md`

다음을 포함한다:

1. **변경 요약**: 무엇이 왜 바뀌는가 (오른쪽 센서 삭제, 3→2) 3~5줄.
2. **영향 매트릭스** (표): 계층 / 파일(legacy 경로) / 판정(변경·일치·검토) / 근거 한 줄.
   - 코드·테스트는 파일 단위, 문서는 섹션/요구사항 ID 단위까지 내려가도 좋다.
3. **일치 예정 목록**: legacy 그대로 둘 파일·항목과 그 이유(예: 근접 센서 인터페이스가 방향 비특정이면 인터페이스 자체는 일치).
4. **단계별 스코프**: `/srs`~`/package` 각 단계가 건드릴 파일 목록 (이후 커맨드의 작업 지시서 역할).
5. **의미 변화 노트**: 회피 로직 단순화 등 행위 수준 변화.

## 4. 작성 규칙

- 모든 경로는 `legacy/`로 시작하는 상대 경로로 적는다.
- 판정 근거는 legacy 실제 코드/문서에 기반한다 (추측 금지).
- ID 체계·클래스명은 **legacy의 것을 그대로 인용**한다 (유지보수이므로 재명명하지 않는다).

## 5. 자기 점검

- [ ] PDF·`CICD-Team7-main/`·`objective.md`를 한 번도 열지 않았는가?
- [ ] legacy의 오른쪽 센서 사용처를 코드·테스트·문서 전 계층에서 빠짐없이 추적했는가?
- [ ] "일치 예정" 항목(예: 방향 비특정 근접 센서 인터페이스)을 명시했는가?
- [ ] 각 후속 단계(`/srs`~`/package`)가 건드릴 파일 목록이 매트릭스에 있는가?

## 6. 로그 종료

`log/${TS}_change-impact.md`에 채움:
- `## 주요 결정`: 어떤 파일을 변경/일치/검토로 판정했는지 핵심 근거, 회피 로직 의미 변화 판단
- `## 생성·수정 파일`: `docs/maintenance/impact.md` 절대 경로
- `## 차이 표기`: 본 단계는 분석이므로 "변경 예정 N건 / 일치 예정 M건 / 검토 K건" 요약
- `## 결과 요약`: 영향 매트릭스 행 수, 계층별 변경 건수
- `## 검증 방법`: `wsl bash -lc "cat docs/maintenance/impact.md | head -60"`
- `## 다음 단계`: `/srs`

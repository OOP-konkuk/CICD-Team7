---
description: legacy/docs/srs/SRS.md를 인계받아 오른쪽 센서 삭제를 반영한 SRS를 docs/srs/SRS.md에 산출한다. legacy 대비 변경/일치를 명시한다. 입력은 legacy + impact.md뿐.
---

# /srs — SRS 유지보수 (오른쪽 센서 삭제 반영)

> **정책 (CLAUDE.md 2·6·7절)**
> 바닥부터 다시 쓰지 않는다. `legacy/docs/srs/SRS.md`를 기준선으로 인계받아 **오른쪽 센서 삭제가 강제하는 부분만 수정**한다.
> 입력은 **legacy + `docs/maintenance/impact.md`뿐**. PDF·`CICD-Team7-main/`·`objective.md`는 열람 금지.
> 수정한 SRS는 legacy 대비 **변경/일치**를 명시해야 한다.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /srs" > log/${TS}_srs.md
echo "" >> log/${TS}_srs.md
echo "## 사용 프롬프트" >> log/${TS}_srs.md
echo "\`/srs\`" >> log/${TS}_srs.md
```

## 1. 입력 (이외 자료 보지 말 것)

- `legacy/docs/srs/SRS.md` — 기준선 SRS (인계 대상)
- `legacy/docs/specs/` — legacy 명세 (해당 요구사항 추적용, 필요시)
- `docs/maintenance/impact.md` — SRS 계층 변경 스코프
- `reference/new_objective.md` — 변경 명세

`reference/`의 PDF·`CICD-Team7-main/`·`objective.md`는 열람 금지.

## 2. 작업 — 인계 후 최소 수정

1. `legacy/docs/srs/SRS.md`를 `docs/srs/SRS.md`로 인계한다 (출발점은 legacy 그대로).
2. `impact.md`가 SRS에 대해 지목한 항목만 수정한다. 전형적 대상:
   - 시스템 컨텍스트·외부 액터: 근접 센서 3→2 (앞/왼쪽)
   - 센서 관련 FR: 오른쪽 센서 입력에 의존하던 요구사항을 좌측 단일 기준으로 수정
   - 회피 관련 FR/UC: 경로 선택을 "좌측 가능 → 좌회전, 아니면 후진"으로 단순화 (legacy의 실제 분기를 근거로)
   - 추적성 매트릭스: 사라진 요구사항/변경된 요구사항 반영
3. **legacy의 ID 체계·요구사항 표현을 그대로 유지**한다. 오른쪽 센서 때문에 바뀌지 않는 FR/NFR/UC는 legacy 문장 그대로 둔다 (재진술 금지).

## 3. 차이 표기 (필수)

`docs/srs/SRS.md` 말미에 **변경 이력 섹션**을 둔다:

| 항목/ID | legacy 대비 | 내용 |
|---|---|---|
| FR-xxx | 변경 | 오른쪽 센서 입력 제거, 좌측 기준으로 단순화 |
| UC-xxx | 변경 | 회피 분기에서 우회전 경로 삭제 |
| NFR-xxx | 일치 | legacy와 동일 |
| ... | ... | ... |

"일치" 항목도 빠짐없이 적어 **무엇을 의도적으로 건드리지 않았는지** 증명한다.

## 4. 자기 점검

- [ ] legacy SRS를 출발점으로 삼고, 오른쪽 센서와 무관한 요구사항을 재작성하지 않았는가?
- [ ] 센서 개수(3→2)·회피 로직 변화가 FR/UC/추적성에 일관되게 반영됐는가?
- [ ] 변경/일치 표가 SRS의 모든 영향 항목을 덮는가?
- [ ] PDF·CICD·objective.md를 열지 않았는가?

## 5. 로그 종료

`log/${TS}_srs.md`에 채움:
- `## 주요 결정`: 어떤 FR/UC를 어떻게 수정했는지, 회피 로직 단순화의 명세 표현
- `## 생성·수정 파일`: `docs/srs/SRS.md`
- `## 차이 표기`: legacy 대비 변경 N건 / 일치 M건 요약 (위 표 압축)
- `## 결과 요약`: 변경된 FR/NFR/UC 수, 삭제 항목 수
- `## 검증 방법`: `wsl bash -lc "diff -u legacy/docs/srs/SRS.md docs/srs/SRS.md | head -80"`
- `## 다음 단계`: `/sdd`

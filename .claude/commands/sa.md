---
description: 수정된 코드에 cppcheck와 clang-tidy를 재실행하고 docs/sa/에 리포트·요약을 저장한다. legacy/docs/sa 리포트 대비 변경/일치를 명시한다.
---

# /sa — 정적 분석 유지보수 (수정 코드 재분석)

> **정책 (CLAUDE.md 2·6·7절)**
> 수정된 `include/`·`src/`에 대해 정적 분석을 **재실행**하고, `legacy/docs/sa`의 기준선 리포트와 비교한다.
> PDF·CICD·objective.md는 열람 금지. 결과는 legacy 리포트 대비 **변경/일치(이슈 증감)**를 명시한다.

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /sa" > log/${TS}_sa.md
echo "" >> log/${TS}_sa.md
echo "## 사용 프롬프트" >> log/${TS}_sa.md
echo "\`/sa\`" >> log/${TS}_sa.md
```

## 1. 사전 조건

- `build/compile_commands.json` 존재 (`CMAKE_EXPORT_COMPILE_COMMANDS=ON`이면 자동). 없으면 `cmake -S . -B build` 먼저.
- 비교 기준: `legacy/docs/sa/{cppcheck.txt,clang-tidy.txt,summary.md}`.

## 2. 분석 실행 (WSL)

### A. cppcheck
```bash
wsl bash -lc "cd '$(pwd)' && cppcheck \
    --enable=all \
    --std=c++17 \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    -I include \
    src test 2> docs/sa/cppcheck.txt"
```

### B. clang-tidy
```bash
wsl bash -lc "cd '$(pwd)' && find src include -name '*.cpp' -o -name '*.h' -o -name '*.hpp' | xargs clang-tidy -p build > docs/sa/clang-tidy.txt 2>&1 || true"
```

## 3. 산출물

- `docs/sa/cppcheck.txt`, `docs/sa/clang-tidy.txt` — raw 출력
- `docs/sa/summary.md` — 요약 + **legacy 대비 비교**:
  - 심각도별 이슈 개수 (error/warning/style/performance/portability)와 **legacy 대비 증감**
  - 우측 센서 삭제로 사라진/새로 생긴 이슈
  - 대응 방침 (수정 / 억제 사유 / 보류)
  - legacy와 동일하게 클린한 항목은 "일치"로 명시

## 4. 수정 정책

- error/warning은 가능한 한 수정 후 재분석. legacy에 없던 새 경고가 우측 센서 삭제에서 비롯됐다면 우선 처리.
- 억제는 코멘트로 사유를 적고 summary에 명시.

## 5. 자기 점검

- [ ] 수정 코드 기준으로 재분석했는가? (legacy가 아닌 최상위 src/include)
- [ ] legacy 리포트 대비 이슈 증감을 명시했는가?
- [ ] 새로 생긴 error/warning을 처리했는가?
- [ ] PDF·CICD·objective.md를 열지 않았는가?

## 6. 로그 종료

`log/${TS}_sa.md`에 채움:
- `## 주요 결정`: 어떤 이슈를 잡고 어떤 것을 억제했는지, legacy 대비 차이 원인
- `## 생성·수정 파일`: 리포트 + (있다면) 코드 수정 파일
- `## 차이 표기`: legacy 리포트 대비 이슈 증감 (변경/일치)
- `## 결과 요약`: 분석 전/후 이슈 수, 잔여 이슈, legacy 대비 델타
- `## 검증 방법`: 위 cppcheck/clang-tidy 명령
- `## 다음 단계`: `/package`

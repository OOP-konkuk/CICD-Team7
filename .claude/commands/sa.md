---
description: cppcheck와 clang-tidy로 정적 분석을 수행하고 docs/sa/에 리포트와 요약을 저장한다.
---

# /sa — Static Analysis

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /sa" > log/${TS}_sa.md
echo "" >> log/${TS}_sa.md
echo "## 사용 프롬프트" >> log/${TS}_sa.md
echo "\`/sa\`" >> log/${TS}_sa.md
```

## 1. 사전 조건

- `build/compile_commands.json` 존재 (CMakeLists.txt에서 `CMAKE_EXPORT_COMPILE_COMMANDS=ON`이면 자동 생성).
- 없으면 먼저 `cmake -S . -B build` 수행.

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
wsl bash -lc "cd '$(pwd)' && find src include -name '*.cpp' -o -name '*.h' | xargs clang-tidy -p build > docs/sa/clang-tidy.txt 2>&1 || true"
```

(`|| true`는 경고가 있어도 분석 자체는 끝까지 돌리기 위함)

## 3. 산출물

- `docs/sa/cppcheck.txt` — cppcheck raw 출력
- `docs/sa/clang-tidy.txt` — clang-tidy raw 출력
- `docs/sa/summary.md` — 사람이 읽을 요약:
  - 심각도별 이슈 개수 (error / warning / style / performance / portability)
  - 각 카테고리에서 대표 이슈 3건과 해당 파일:라인
  - 대응 방침 (수정 / 억제 사유 / 보류)
  - SRS NFR(유지보수성 등) 충족 여부

## 4. 수정 정책

- **error / warning 등급**은 가능한 한 수정한다. 수정 후 재분석.
- **style** 카테고리는 일관성을 해치는 것만 수정.
- 억제(suppression)는 코멘트로 사유를 적고 SDD 또는 본 요약에 명시.

## 5. 로그 종료

`log/${TS}_sa.md`에 채움:
- `## 주요 결정`: 어떤 카테고리를 어디까지 잡았는지, 억제 사유
- `## 생성·수정 파일`: 리포트 + 코드 수정 파일
- `## 결과 요약`: 분석 전/후 이슈 수, 잔여 이슈
- `## 검증 방법`: 위 cppcheck/clang-tidy 명령
- `## 다음 단계`: `/package`

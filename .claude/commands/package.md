---
description: 모든 산출물의 일관성·재현성을 점검하고 README를 최종화한다. 조교가 한 줄 명령으로 빌드/테스트할 수 있는 상태로 마무리.
---

# /package — Final Packaging & Reproducibility Check

## 0. 로그 시작

```bash
TS=$(date +%Y-%m-%d_%H%M)
echo "# /package" > log/${TS}_package.md
echo "" >> log/${TS}_package.md
echo "## 사용 프롬프트" >> log/${TS}_package.md
echo "\`/package\`" >> log/${TS}_package.md
```

## 1. 산출물 점검 체크리스트

| 항목 | 위치 | 점검 |
|---|---|---|
| 명세 재증류 | `docs/specs/{system-overview,requirements,use-cases}.md` | 3개 파일 존재 + `requirements.md`에 추적성 표 |
| SRS | `docs/srs/SRS.md` | 재증류된 새 ID로 작성, UC 목록 + 추적성 매트릭스 |
| SDD | `docs/sdd/SDD.md` | mermaid 다이어그램 + 시그니처, PDF Domain Model 답습 없음 |
| Code | `include/`, `src/`, `main.cpp` | 빌드 경고 0, SDD 매핑 표와 일치 |
| UT | `test/unit/*.cpp` | 모두 통과, 실제 클래스명 기준 |
| Simulator | `test/simulator/*` | SDD 추상화 인터페이스에 대응 + 환경 주입 API |
| ST | `test/system/*.cpp` | 모두 통과, UC 커버리지 100% |
| SA | `docs/sa/{cppcheck.txt,clang-tidy.txt,summary.md}` | 3개 파일 |
| 로그 | `log/*.md` | 각 슬래시 커맨드 1개 이상, 주요 결정에 PDF 대비 차이 명시 |
| README | `README.md` | 위 한 줄 명령 동작 확인 |

## 2. 클린 빌드 재현 (WSL)

깨끗한 상태에서 한 줄 명령이 통과해야 한다:

```bash
wsl bash -lc "cd '$(pwd)' && rm -rf build && cmake -S . -B build && cmake --build build -j 2>&1 | tee /tmp/rvc_package_build.log && ctest --test-dir build --output-on-failure 2>&1 | tee /tmp/rvc_package_test.log"
```

빌드 경고 0, 테스트 100% 통과 확인.

## 3. README 최종화

`README.md`를 다음 기준으로 점검·수정:

- 한 줄 빌드/테스트 명령이 위 클린 빌드 명령과 일치하는지
- 산출물 위치 표가 실제 파일과 일치하는지
- apt 패키지 목록이 실제 의존성과 일치하는지
- `log/`의 슬래시 커맨드 호출 순서와 일치하는 워크플로우 설명

또한 `README.md`에 다음 섹션 추가/갱신:
- **빌드/테스트 결과 요약** — `/tmp/rvc_package_build.log` 와 `/tmp/rvc_package_test.log`의 통계 (빌드 시간, 테스트 수, 통과율)
- **AI 작동 기록 인덱스** — `log/` 폴더의 모든 마크다운 파일을 순서대로 링크

## 4. 발표 자료 인덱스

`log/INDEX.md`를 생성 (또는 갱신):
- 슬래시 커맨드 실행 순서별 로그 링크
- 각 로그의 "주요 결정" 1줄 요약
- 발표자가 이 인덱스 하나로 모든 의사결정 흐름을 따라갈 수 있어야 함

## 5. 로그 종료

`log/${TS}_package.md`에 채움:
- `## 주요 결정`: 최종 점검에서 보완한 항목, README 변경 요점
- `## 생성·수정 파일`: README.md, log/INDEX.md, (있다면) 보완된 산출물
- `## 결과 요약`: 클린 빌드 시간, 테스트 통과율, 산출물 총 LOC/문서 페이지
- `## 검증 방법`: 위 클린 빌드 한 줄 명령
- `## 다음 단계`: 제출

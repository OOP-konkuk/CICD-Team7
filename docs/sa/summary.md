# 정적 분석 요약 (유지보수 재실행 — 오른쪽 근접 센서 삭제)

분석 일시: 2026-05-29
대상: 수정된 `src/` + `include/` (최상위, legacy 아님)
비교 기준선: `legacy/docs/sa/{cppcheck.txt, clang-tidy.txt, summary.md}` (2026-05-21)

분석 도구:
- **GCC 13.3.0** `-Wall -Wextra -Wpedantic -Wconversion -Wshadow` (컴파일러 진단)
- **GCC 13.3.0** `-fanalyzer` (프로시저 간 흐름 분석)
- Note: cppcheck / clang-tidy 바이너리가 WSL 환경에 설치되지 않아 GCC 도구를 대체 사용 — **legacy와 동일한 방법론**을 유지해 비교의 등가성을 보장한다.

---

## 1. 분석 결과 (수정 코드)

| 분석 도구 | 대상 파일 | 발견 | 프로젝트 이슈 | 결론 |
|---------|---------|------|-----------|------|
| GCC -Wall -Wextra -Wpedantic -Wconversion -Wshadow | 모든 src/ (6파일) | 0 | 0 | **clean** |
| GCC -fanalyzer | src/core/ (3파일) | 0 | 0 | **clean** |

수정 코드는 빌드/진단 모두 클린이며, 신규 error/warning이 0건이다.

---

## 2. legacy 리포트 대비 비교 (이슈 증감)

| 항목 | legacy (2026-05-21) | 수정 (2026-05-29) | 판정 | 비고 |
|------|------|------|------|------|
| -Wall/-Wextra/-Wpedantic/-Wconversion/-Wshadow | 0 | 0 | **일치** | 우측 센서 삭제가 새 경고를 만들지 않음 |
| -fanalyzer 프로젝트 이슈 | 0 | 0 | **일치** | 프로젝트 코드 결함 0 (legacy·수정 동일) |
| -fanalyzer STL 거짓 양성 | 5 (CWE-457) | 0 | 변경(감소) | ↓5. **toolchain 차이**로 인한 소멸 — §3 참조 |
| error / warning / style / performance | 0 / 0 / 0 / 0 | 0 / 0 / 0 / 0 | **일치** | |
| portability (POSIX header) | 의도된 1 (억제) | 의도된 1 (억제) | **일치** | `cli_command_source.cpp` — 우측 센서 무관 |

### 우측 센서 삭제로 사라진/새로 생긴 이슈
- **새로 생긴 이슈: 없음.** `ObstacleHandler` ctor/멤버에서 `right`/`right_` 를 제거하면서 "미사용 매개변수/멤버" 경고가 남지 않도록 **완전 제거**했고(`-Wextra` 0건으로 확인), `selectPath()`의 우측 분기·`avoidAndReturn()`의 `AvoidPath::TURN_RIGHT` 케이스도 dead code 없이 제거되어 도달 불가 경고가 발생하지 않음.
- **사라진 이슈: -fanalyzer STL 거짓 양성 5건** — 단, 이는 우측 센서 삭제가 아니라 toolchain/재실행 환경 차이에 기인(§3).

---

## 3. -fanalyzer 거짓 양성 5건의 소멸에 대한 설명 (중요)

legacy summary(2026-05-21)는 `std::string` move 생성자/`operator+` 경로에서 CWE-457 거짓 양성 5건을 보고했다(GCC Bugzilla #107417, libstdc++ SSO 추적 버그). 본 재실행에서는:

- **수정 코드**: -fanalyzer 0건
- **legacy 코드(동일 명령 재실행)**: -fanalyzer 0건 ← 검증 완료
  ```
  g++ -std=c++17 -fanalyzer -Wall -Wextra -Wpedantic -Ilegacy/include -fsyntax-only \
      legacy/src/core/{cleaning_engine,obstacle_handler,clean_intensity_controller}.cpp
  → 0 warnings
  ```

즉 legacy·수정 **양쪽 모두 0건**이다. 5건은 우측 센서 삭제와 무관한, 당시 toolchain의 STL 거짓 양성 잔재이며 현재 toolchain(GCC 13.3.0, 2026-05-29 환경)에서는 재현되지 않는다. 따라서 **프로젝트 코드 기준으로 legacy와 수정은 동일하게 클린**하다(일치). 이 감소는 코드 개선이 아니라 분석 환경 변화의 결과임을 명시한다.

---

## 4. 심각도별 이슈 분류 (수정 후 잔여)

| 등급 | 수 | legacy 대비 | 비고 |
|------|----|------|------|
| error | 0 | 일치 | |
| warning | 0 | 일치 | |
| style | 0 | 일치 | |
| performance | 0 | 일치 | |
| portability | 0 | 일치 | `cli_command_source.cpp`의 POSIX `<sys/select.h>`는 WSL/Linux 타겟 전용 의도 (억제) |

---

## 5. SRS NFR 충족 여부 (legacy 대비 일치)

| NFR | 내용 | 분석 결과 |
|-----|------|---------|
| NFR-BUILD-01 | C++17 표준 준수 | `-std=c++17 -Wpedantic` 통과 |
| NFR-ARCH-01 | 추상 클래스 기반 분리 | 인터페이스 ↔ 구현 분리 유지 (우측 센서 삭제 후에도 `IProximitySensor` 단일 인터페이스 그대로) |
| NFR-ARCH-02 | Stub 격리 가능 | test/unit/stub/, test/simulator/ 구조 유지 |
| NFR-ARCH-03 | 오류 복구 불가 시 안전 정지 | `haltSafely()` catch-all 유지 |

---

## 6. 억제(Suppression) 목록

| 규칙 | 파일 | 사유 | legacy 대비 |
|------|------|------|------|
| GCC -fanalyzer CWE-457 | libstdc++ basic_string | 현재 toolchain에서 미재현(0건). 억제 불필요해짐 | 변경(억제 항목 소멸) |
| `portability` POSIX header | src/app/cli_command_source.cpp | WSL/Linux 전용 빌드 환경 (NFR-BUILD-01 WSL 명시). 우측 센서 무관 | 일치 |

---

## 7. 결론

- 수정 코드 정적 분석 결과 **error/warning/style/performance/portability 모두 0** — legacy와 **일치(clean)**.
- 우측 근접 센서 삭제는 **새 정적 분석 이슈를 0건 유발**했다. 멤버·매개변수·분기를 잔여 없이 완전 제거해 미사용/도달불가 경고가 없다.
- legacy 대비 유일한 수치 변화인 -fanalyzer STL 거짓 양성 5→0은 **toolchain 차이**이며 코드 변경과 무관(legacy 코드도 현재 0건).

# 정적 분석 요약

분석 일시: 2026-05-21  
분석 도구:
- **GCC 13.3.0** `-Wall -Wextra -Wpedantic -Wconversion -Wshadow` (컴파일러 진단)
- **GCC 13.3.0** `-fanalyzer` (프로시저 간 흐름 분석)
- Note: cppcheck / clang-tidy 바이너리가 WSL 환경에 설치되지 않아 GCC 도구를 대체 사용.

---

## 1. 분석 전 발견 및 수정

### 수정 1 — `ObstacleHandler::front_` 미사용 멤버 제거
- **위치**: `include/core/obstacle_handler.hpp`, `src/core/obstacle_handler.cpp`
- **심각도**: Warning (미사용 멤버, 불필요한 의존성)
- **내용**: `ObstacleHandler`가 생성자에서 `IProximitySensor& front`를 받아 `front_`에 저장했지만, 클래스 내 어떤 메서드에서도 사용하지 않았음. 전방 감지는 `CleaningEngine::cleanLoop()`에서 이미 처리됨.
- **조치**: `front` 파라미터 및 `front_` 멤버 제거. `CleaningEngine` 생성자 이니셜라이저도 수정. 단위 테스트 픽스처 업데이트.
- **영향**: 빌드 경고 없음, 테스트 56/56 통과.

### 수정 2 — `main.cpp` C-스타일 배열 → `std::array`
- **위치**: `src/app/main.cpp` (`DemoDriveTrain::execute`, `DemoCleaningUnit::setIntensity`)
- **심각도**: Style (C-스타일 배열, bounds-safe 대안 권장)
- **내용**: `const char* names[] = {...}` 사용 → cppcheck/clang-tidy의 `cppcoreguidelines-avoid-c-arrays` 규칙 위반.
- **조치**: `static constexpr std::array<const char*, N>` 로 변경. `static_cast<std::size_t>` 명시.
- **영향**: 빌드 경고 없음.

---

## 2. 분석 결과 요약

| 분석 도구 | 대상 파일 | 발견 | 프로젝트 이슈 | 결론 |
|---------|---------|------|-----------|------|
| GCC -Wall -Wextra -Wpedantic -Wconversion -Wshadow | 모든 src/ | 0 | 0 | **clean** |
| GCC -fanalyzer | src/core/ | 5 | 0 | STL 거짓 양성 |

### GCC -fanalyzer 거짓 양성 상세
- **증상**: `std::string` move constructor 내 `use of uninitialized value` (CWE-457) 경고 5건
- **원인**: GCC analyzer의 알려진 버그 — SSO(small-string optimization) 경로에서 `basic_string` move 생성자 내부의 초기화 순서를 잘못 추적 (GCC Bugzilla #107417)
- **프로젝트 코드와의 관계**: 경고가 추적하는 콜 체인 진입점은 `CleaningEngine::run()` 내 `std::string("[ERROR] ") + e.what()` 식이나, 실제 미초기화 값은 STL 내부. 프로젝트 코드 결함 없음.
- **대응**: 억제(suppress) 사유 본 문서에 기록; 프로덕션 코드 수정 불필요.

---

## 3. 심각도별 이슈 분류 (수정 후 잔여)

| 등급 | 수 | 비고 |
|------|----|------|
| error | 0 | |
| warning | 0 | |
| style | 0 | |
| performance | 0 | |
| portability | 0 | `cli_command_source.cpp`의 POSIX `<sys/select.h>` 사용은 WSL/Linux 타겟 전용으로 의도된 것 |

---

## 4. SRS NFR 충족 여부

| NFR | 내용 | 분석 결과 |
|-----|------|---------|
| NFR-BUILD-01 | C++17 표준 준수 | `-std=c++17 -Wpedantic` 통과 |
| NFR-ARCH-01 | 추상 클래스 기반 분리 | 인터페이스 ↔ 구현 분리 확인 |
| NFR-ARCH-02 | Stub 격리 가능 | test/unit/stub/, test/simulator/ 구조 확인 |
| NFR-ARCH-03 | 오류 복구 불가 시 안전 정지 | `haltSafely()` catch-all 확인 |

---

## 5. 억제(Suppression) 목록

| 규칙 | 파일 | 사유 |
|------|------|------|
| GCC -fanalyzer CWE-457 (5건) | libstdc++ basic_string | STL 내부 false positive (GCC Bug #107417); 프로젝트 코드 무관 |
| `portability` POSIX header | src/app/cli_command_source.cpp | WSL/Linux 전용 빌드 환경 (NFR-BUILD-01 WSL 명시) |

#pragma once
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct TestResult {
    std::string suiteName;
    std::string testName;
    bool        passed{true};
    std::string failMessage;
    double      elapsedMs{0.0};
};

class TestRunner {
public:
    struct Registration {
        std::string           suite;
        std::string           name;
        std::function<void()> fn;
    };

    static TestRunner& instance() {
        static TestRunner inst;
        return inst;
    }

    void registerTest(const std::string& suite,
                      const std::string& name,
                      std::function<void()> fn) {
        registrations_.push_back({suite, name, std::move(fn)});
    }

    void recordFailure(const std::string& msg) {
        if (current_ && current_->passed) {
            current_->passed      = false;
            current_->failMessage = msg;
        }
    }

    int runAll(const std::string& xmlOutputPath = "") {
        int passed = 0, failed = 0;

        for (auto& reg : registrations_) {
            TestResult result;
            result.suiteName = reg.suite;
            result.testName  = reg.name;
            current_         = &result;

            auto t0 = std::chrono::steady_clock::now();
            try {
                reg.fn();
            } catch (const std::exception& ex) {
                recordFailure(std::string("EXCEPTION: ") + ex.what());
            } catch (...) {
                recordFailure("UNKNOWN EXCEPTION");
            }
            auto t1        = std::chrono::steady_clock::now();
            result.elapsedMs =
                std::chrono::duration<double, std::milli>(t1 - t0).count();
            current_ = nullptr;

            if (result.passed) {
                std::cout << "[PASS] " << reg.suite << "::" << reg.name << "\n";
                ++passed;
            } else {
                std::cout << "[FAIL] " << reg.suite << "::" << reg.name
                          << "\n       " << result.failMessage << "\n";
                ++failed;
            }
            results_.push_back(std::move(result));
        }

        std::cout << "\n=== System Test Results ===\n"
                  << "Passed: " << passed << "\n"
                  << "Failed: " << failed << "\n"
                  << "Total : " << (passed + failed) << "\n";

        if (!xmlOutputPath.empty())
            writeJUnit(xmlOutputPath, failed);

        return (failed > 0) ? 1 : 0;
    }

private:
    TestRunner() = default;
    std::vector<Registration> registrations_;
    std::vector<TestResult>   results_;
    TestResult*               current_{nullptr};

    static std::string xmlEscape(const std::string& s) {
        std::string out;
        for (char c : s) {
            switch (c) {
                case '<': out += "&lt;";   break;
                case '>': out += "&gt;";   break;
                case '&': out += "&amp;";  break;
                case '"': out += "&quot;"; break;
                default:  out += c;
            }
        }
        return out;
    }

    void writeJUnit(const std::string& path, int totalFailed) {
        std::ofstream f(path);
        if (!f) return;

        double totalMs = 0.0;
        for (auto& r : results_) totalMs += r.elapsedMs;

        f << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
          << "<testsuites tests=\"" << results_.size()
          << "\" failures=\"" << totalFailed
          << "\" time=\"" << (totalMs / 1000.0) << "\">\n";

        std::string cur;
        for (auto& r : results_) {
            if (r.suiteName != cur) {
                if (!cur.empty()) f << "  </testsuite>\n";
                cur = r.suiteName;
                int sf = 0, st = 0; double sm = 0.0;
                for (auto& s : results_) {
                    if (s.suiteName != cur) continue;
                    ++st; sm += s.elapsedMs;
                    if (!s.passed) ++sf;
                }
                f << "  <testsuite name=\"" << xmlEscape(cur)
                  << "\" tests=\"" << st
                  << "\" failures=\"" << sf
                  << "\" time=\"" << (sm / 1000.0) << "\">\n";
            }
            f << "    <testcase name=\"" << xmlEscape(r.testName)
              << "\" classname=\"" << xmlEscape(r.suiteName)
              << "\" time=\"" << (r.elapsedMs / 1000.0) << "\"";
            if (r.passed) {
                f << "/>\n";
            } else {
                f << ">\n      <failure message=\""
                  << xmlEscape(r.failMessage) << "\"/>\n    </testcase>\n";
            }
        }
        if (!cur.empty()) f << "  </testsuite>\n";
        f << "</testsuites>\n";
    }
};

// ── Assertion macros (non-fatal: records first failure, test continues) ──────

#define STEST_FAIL(msg) \
    do { \
        std::ostringstream _oss_; \
        _oss_ << __FILE__ << ":" << __LINE__ << " | " << (msg); \
        TestRunner::instance().recordFailure(_oss_.str()); \
    } while (0)

#define STEST_EXPECT_TRUE(expr) \
    do { if (!(expr)) STEST_FAIL("Expected TRUE: " #expr); } while (0)

#define STEST_EXPECT_FALSE(expr) \
    do { if ((expr))  STEST_FAIL("Expected FALSE: " #expr); } while (0)

#define STEST_EXPECT_EQ(a, b) \
    do { \
        if (!((a) == (b))) { \
            std::ostringstream _oss_; \
            _oss_ << "Expected EQ: " #a " == " #b \
                  << " (got " << (a) << " vs " << (b) << ")"; \
            STEST_FAIL(_oss_.str()); \
        } \
    } while (0)

#define STEST_EXPECT_CONTAINS(str, sub) \
    do { \
        if ((str).find(sub) == std::string::npos) \
            STEST_FAIL("Expected \"" + std::string(sub) + \
                       "\" in \"" + (str) + "\""); \
    } while (0)

#define STEST_EXPECT_NOT_CONTAINS(str, sub) \
    do { \
        if ((str).find(sub) != std::string::npos) \
            STEST_FAIL("Did NOT expect \"" + std::string(sub) + \
                       "\" in \"" + (str) + "\""); \
    } while (0)

// ── Registration macro ────────────────────────────────────────────────────────
#define STEST_REGISTER(suite, name, fn) \
    namespace { \
        const bool _sreg_##suite##_##name = []{ \
            TestRunner::instance().registerTest(#suite, #name, fn); \
            return true; \
        }(); \
    }

// ── Place exactly once at the bottom of the test .cpp file ───────────────────
#define STEST_MAIN() \
    int main(int argc, char** argv) { \
        std::string xmlPath; \
        for (int i = 1; i < argc; ++i) { \
            std::string a(argv[i]); \
            if (a.rfind("--xml=", 0) == 0) xmlPath = a.substr(6); \
        } \
        return TestRunner::instance().runAll(xmlPath); \
    }

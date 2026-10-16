#pragma once

#include <cmath>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace igi::test {

struct TestCase {
    std::string suite_name;
    std::string test_name;
    std::function<void()> func;
};

class TestRegistry {
public:
    static TestRegistry& instance() noexcept {
        static TestRegistry reg;
        return reg;
    }

    void register_test(std::string suite, std::string name, std::function<void()> fn) {
        tests_.push_back({std::move(suite), std::move(name), std::move(fn)});
    }

    [[nodiscard]] const std::vector<TestCase>& tests() const noexcept {
        return tests_;
    }

private:
    std::vector<TestCase> tests_;
};

struct TestFailureException : public std::exception {
    std::string message;
    TestFailureException(std::string msg) : message(std::move(msg)) {}
    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};

inline int run_all_tests() {
    const auto& tests = TestRegistry::instance().tests();
    int passed = 0;
    int failed = 0;

    std::cout << "\n==================================================\n";
    std::cout << " Running " << tests.size() << " Project Open IGI automated tests...\n";
    std::cout << "==================================================\n";

    std::string current_suite;
    for (const auto& t : tests) {
        if (t.suite_name != current_suite) {
            current_suite = t.suite_name;
            std::cout << "\n[" << current_suite << "]\n";
        }
        try {
            t.func();
            std::cout << "  [PASS] " << t.test_name << "\n";
            ++passed;
        } catch (const TestFailureException& ex) {
            std::cout << "  [FAIL] " << t.test_name << "\n         " << ex.what() << "\n";
            ++failed;
        } catch (const std::exception& ex) {
            std::cout << "  [FAIL] " << t.test_name << "\n         Unexpected std::exception: " << ex.what() << "\n";
            ++failed;
        } catch (...) {
            std::cout << "  [FAIL] " << t.test_name << "\n         Unknown non-std exception\n";
            ++failed;
        }
    }

    std::cout << "\n==================================================\n";
    std::cout << " Test Summary: " << passed << " passed, " << failed << " failed (Total: " << tests.size() << ")\n";
    std::cout << "==================================================\n";

    return failed == 0 ? 0 : 1;
}

#define TEST_CASE(suite, name) \
    static void _igi_test_func_##suite##_##name(); \
    namespace { \
        struct _igi_test_reg_##suite##_##name { \
            _igi_test_reg_##suite##_##name() { \
                ::igi::test::TestRegistry::instance().register_test( \
                    #suite, #name, &_igi_test_func_##suite##_##name); \
            } \
        } _igi_test_inst_##suite##_##name; \
    } \
    static void _igi_test_func_##suite##_##name()

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: (" #expr ") at " << __FILE__ << ":" << __LINE__; \
            throw ::igi::test::TestFailureException(_oss.str()); \
        } \
    } while (false)

#define ASSERT_FALSE(expr) \
    do { \
        if (expr) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: !(" #expr ") at " << __FILE__ << ":" << __LINE__; \
            throw ::igi::test::TestFailureException(_oss.str()); \
        } \
    } while (false)

#define ASSERT_EQ(val1, val2) \
    do { \
        if (!((val1) == (val2))) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: (" #val1 " == " #val2 ") [" << (val1) << " != " << (val2) << "] at " \
                 << __FILE__ << ":" << __LINE__; \
            throw ::igi::test::TestFailureException(_oss.str()); \
        } \
    } while (false)

#define ASSERT_NE(val1, val2) \
    do { \
        if ((val1) == (val2)) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: (" #val1 " != " #val2 ") at " << __FILE__ << ":" << __LINE__; \
            throw ::igi::test::TestFailureException(_oss.str()); \
        } \
    } while (false)

#define ASSERT_NEAR(val1, val2, eps) \
    do { \
        const auto _v1 = (val1); \
        const auto _v2 = (val2); \
        const auto _diff = std::abs(_v1 - _v2); \
        if (_diff > (eps)) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: std::abs(" #val1 " - " #val2 ") <= " #eps \
                 << " [|" << _v1 << " - " << _v2 << "| = " << _diff << " > " << (eps) << "] at " \
                 << __FILE__ << ":" << __LINE__; \
            throw ::igi::test::TestFailureException(_oss.str()); \
        } \
    } while (false)

#define ASSERT_THROWS(expr, ExceptionType) \
    do { \
        bool _threw = false; \
        try { \
            (void)(expr); \
        } catch (const ExceptionType&) { \
            _threw = true; \
        } catch (...) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: (" #expr ") threw unexpected exception type instead of " #ExceptionType \
                 << " at " << __FILE__ << ":" << __LINE__; \
            throw ::igi::test::TestFailureException(_oss.str()); \
        } \
        if (!_threw) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: (" #expr ") expected to throw " #ExceptionType \
                 << " but did not throw at " << __FILE__ << ":" << __LINE__; \
            throw ::igi::test::TestFailureException(_oss.str()); \
        } \
    } while (false)

#define ASSERT_NO_THROW(expr) \
    do { \
        try { \
            (void)(expr); \
        } catch (const std::exception& _e) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: (" #expr ") threw unexpected exception: " << _e.what() \
                 << " at " << __FILE__ << ":" << __LINE__; \
            throw ::igi::test::TestFailureException(_oss.str()); \
        } catch (...) { \
            std::ostringstream _oss; \
            _oss << "Assertion failed: (" #expr ") threw unknown exception at " \
                 << __FILE__ << ":" << __LINE__; \
            throw ::igi::test::TestFailureException(_oss.str()); \
        } \
    } while (false)

} // namespace igi::test

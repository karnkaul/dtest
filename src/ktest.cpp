#include <array>
#include <chrono>
#include <exception>
#include <filesystem>
#include <future>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <ktest/ktest.hpp>

namespace ktest {
using namespace std::chrono;

namespace {
std::ostream& fmt_str(std::ostream& out, std::string_view fmt) { return out << fmt; }
template <typename Arg, typename... Args>
std::ostream& fmt_str(std::ostream& out, std::string_view fmt, Arg const& arg, Args const&... args) {
	if (auto const idx = fmt.find("{}"); idx < fmt.size()) { return fmt_str(out << fmt.substr(0, idx) << arg, fmt.substr(idx + 2), args...); }
	return fmt_str(out, fmt);
}
template <typename... Args>
void log(std::string_view fmt, Args const&... args) {
	std::stringstream str;
	fmt_str(str, fmt, args...);
	std::cout << str.str();
}

struct test_failed : std::exception {};

class runner_t {
  public:
	inline static std::mutex s_mutex;
	inline static std::vector<test_t*> s_tests;
	inline static std::vector<test_t*> s_failed;

	static void run(test_t* test) {
		try {
			test->func(test->helper);
			if (!test->helper.fails.empty()) {
				auto lock = std::scoped_lock(s_mutex);
				s_failed.push_back(test);
			}
		} catch (test_failed const&) {
			auto lock = std::scoped_lock(s_mutex);
			s_failed.push_back(test);
		}
		std::stringstream str;
		if (!test->helper.fails.empty()) {
			fmt_str(str, "[ FAIL ] {}\n", test->name);
			for (auto const& expr : test->helper.fails) {
				auto const file = std::filesystem::path(expr.file).filename().string();
				fmt_str(str, " \t{}: {}  ({} | {})\n", (expr.required ? "required" : "expected"), expr.str, file, expr.line);
			}
			std::cerr << str.str();
		} else {
			fmt_str(str, "[ pass ] {}\n", test->name);
			std::cout << str.str();
		}
	}
};

std::string pretty_time(duration<float, std::ratio<1, 1000>> dt) {
	std::stringstream str;
	str.setf(std::ios::fixed);
	str << std::setprecision(1);
	if (dt.count() > 3000.0f) {
		str << (dt.count() / 1000.0f) << 's';
	} else {
		str << dt.count() << "ms";
	}
	return str.str();
}

std::size_t s_test_idx = 0;
std::array<test_t*, KTEST_MAX_TESTS> g_tests = {};
} // namespace

test_t::test_t(std::string_view name, func_t func) : name(name), func(func) {
	if (s_test_idx == g_tests.size()) {
		std::cerr << "ERROR: Max test limit exceeded: " << g_tests.size() << '\n';
		std::terminate();
	}
	g_tests[s_test_idx++] = this;
}

void helper_t::check(expr_t const& expr, bool required) {
	if (!expr.pass) {
		fails.push_back(expr);
		if (required) {
			fails.back().required = true;
			throw test_failed{};
		}
	}
}

int run_tests(bool async) {
	runner_t::s_tests.clear();
	for (auto test : g_tests) {
		if (test) { runner_t::s_tests.push_back(test); }
	}
	std::vector<std::future<void>> futures;
	if (async) { futures.reserve(runner_t::s_tests.size()); }
	auto const start = steady_clock::now();
	log("[======] starting {} tests\n", runner_t::s_tests.size());
	for (auto test : runner_t::s_tests) {
		if (async) {
			futures.push_back(std::async(&runner_t::run, test));
		} else {
			runner_t::run(test);
		}
	}
	futures.clear();
	auto const dt = duration_cast<duration<float, std::ratio<1, 1000>>>(steady_clock::now() - start);
	auto const total = runner_t::s_tests.size();
	log("[======] completed {} tests ({})\n", total, pretty_time(dt));
	auto const failed = runner_t::s_failed.size();
	auto const passed = total - runner_t::s_failed.size();
	log("[ pass ] {} / {}\n", passed, total);
	if (failed > 0) {
		log("[ FAIL ] {} / {}\n", failed, total);
		return KTEST_FAIL_RETURN_CODE;
	}
	return 0;
}
} // namespace ktest

#include <cassert>
#include <chrono>
#include <exception>
#include <filesystem>
#include <future>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <dumb_test/dtest.hpp>

namespace dtest {
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
	inline static std::vector<atest_t*> s_tests;
	inline static std::vector<atest_t*> s_failed;

	static void run(atest_t* test) {
		try {
			(*test)();
			if (!test->fails.empty()) {
				auto lock = std::scoped_lock(s_mutex);
				s_failed.push_back(test);
			}
		} catch (test_failed const&) {
			auto lock = std::scoped_lock(s_mutex);
			s_failed.push_back(test);
		}
		std::stringstream str;
		if (!test->fails.empty()) {
			fmt_str(str, "[ FAIL ] {}\n", test->name);
			for (auto const& e : test->fails) {
				auto const file = std::filesystem::path(e.file).filename().string();
				std::string_view const exp = e.required ? "required" : "expected";
				if (!e.a.empty() && !e.b.empty()) {
					fmt_str(str, " \t{}: {} {} {} ({} | {})\n", exp, e.a, detail::op_str(e.op), e.b, file, e.line);
				} else {
					fmt_str(str, " \t{}: {}  ({} | {})\n", exp, e.expr, file, e.line);
				}
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

struct {
	atest_t* head{};
	atest_t* tail{};
} g_ends;
} // namespace

atest_t::atest_t(std::string_view name) noexcept : name(name) {
	if (!g_ends.tail) {
		g_ends.tail = g_ends.head = this;
		return;
	}
	g_ends.tail->next = this;
	g_ends.tail = g_ends.tail->next;
	assert(g_ends.tail->next == nullptr);
}

void atest_t::check(expr_t expr, bool required) {
	if (!expr.pass) {
		fails.push_back(std::move(expr));
		if (required) { throw test_failed{}; }
	}
}

int run_tests(bool async) {
	runner_t::s_tests.clear();
	runner_t::s_failed.clear();
	for (auto it = g_ends.head; it != nullptr; it = it->next) { runner_t::s_tests.push_back(it); }
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
		return DTEST_FAIL_RETURN_CODE;
	}
	return 0;
}
} // namespace dtest

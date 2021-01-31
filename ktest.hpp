// KT header-only library
// Requirements: C++17

#pragma once
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace kt {
///
/// \brief Helper class to expect / assert conditions on pairs of expressions
///
class executor_t;
///
/// \brief Prototype of a test function
///
using func_t = void (*)(executor_t const& exec);
///
/// \brief Map of names to test functions
///
using test_map_t = std::unordered_map<std::string_view, func_t>;

///
/// \brief Helper class to expect / assert conditions on pairs of expressions
///
class executor_t {
  public:
	///
	/// \brief Fail test if t == u fails, unwind
	///
	template <typename T, typename U>
	void assert_eq(T const& t, U const& u) const;
	///
	/// \brief Fail test if t != u fails, unwind
	///
	template <typename T, typename U>
	void assert_neq(T const& t, U const& u) const;
	///
	/// \brief Fail test if t == u fails, continue
	///
	template <typename T, typename U>
	void expect_eq(T const& t, U const& u) const noexcept;
	///
	/// \brief Fail test if t != u fails, continue
	///
	template <typename T, typename U>
	void expect_neq(T const& t, U const& u) const noexcept;

  private:
	executor_t(std::string_view name);
	void fail() const;

	std::string_view m_name;
	mutable bool m_pass = true;

	friend class runner_t;
};

///
/// \brief Tests runner
///
class runner_t {
  public:
	runner_t(test_map_t tests);

	int run(bool async);

  private:
	test_map_t m_tests;
	int m_ret = 0;
};

// impl

namespace detail {
struct test_fail : std::runtime_error {
	test_fail() : std::runtime_error("failed") {
	}
};
struct equals {
	template <typename T, typename U>
	constexpr bool operator()(T const& t, U const& u) const noexcept {
		return t == u;
	}
};
struct not_equals {
	template <typename T, typename U>
	constexpr bool operator()(T const& t, U const& u) const noexcept {
		return t != u;
	}
};
} // namespace detail

inline runner_t::runner_t(test_map_t tests) : m_tests(std::move(tests)) {
}
inline int runner_t::run(bool async) {
	std::vector<std::future<void>> futures;
	if (async) {
		futures.reserve(m_tests.size());
	}
	std::atomic<std::size_t> passed, failed;
	passed.store(0);
	failed.store(0);
	try {
		for (auto const& kvp : m_tests) {
			auto run_test = [this, kvp, &passed, &failed]() {
				executor_t exec(kvp.first);
				kvp.second(exec);
				if (exec.m_pass) {
					++passed;
				} else {
					++failed;
					m_ret = 1;
				}
			};
			if (async) {
				futures.push_back(std::async(run_test));
			} else {
				run_test();
			}
		}
	} catch (detail::test_fail const&) {
		++failed;
		m_ret = 1;
	}
	for (auto& future : futures) {
		if (future.valid()) {
			future.get();
		}
	}
	std::cout << "[dtest] tests: " << m_tests.size() << " | " << passed.load() << " passed | " << failed.load() << " failed" << std::endl;
	return m_ret;
}
inline executor_t::executor_t(std::string_view name) : m_name(name) {
}
inline void executor_t::fail() const {
	std::string log = "[dtest] ";
	log += m_name;
	log += " failed!\n";
	std::cerr << log;
	m_pass = false;
}
template <typename T, typename U>
void executor_t::expect_eq(T const& t, U const& u) const noexcept {
	if (!(t == u)) {
		fail();
	}
}
template <typename T, typename U>
void executor_t::expect_neq(T const& t, U const& u) const noexcept {
	if (!(t != u)) {
		fail();
	}
}
template <typename T, typename U>
void executor_t::assert_eq(T const& t, U const& u) const {
	if (!(t == u)) {
		fail();
		throw detail::test_fail();
	}
}
template <typename T, typename U>
void executor_t::assert_neq(T const& t, U const& u) const {
	if (!(t != u)) {
		fail();
		throw detail::test_fail();
	}
}
} // namespace kt

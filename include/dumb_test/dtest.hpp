#pragma once

#include <cstdint>
#include <vector>
#include <dumb_test/detail/operators.hpp>

///
/// \brief Usage: TEST(foo) { // test body... }
/// Create and register a new test
///
#define TEST(name)                                                                                                                                             \
	struct test_##name : ::dtest::atest_t {                                                                                                                    \
		test_##name() : ::dtest::atest_t(#name) {}                                                                                                             \
		void operator()() override;                                                                                                                            \
	};                                                                                                                                                         \
	test_##name g_##name;                                                                                                                                      \
	void test_##name::operator()()

#define DTEST_CHECK(a, b, op, req) this->check<op>(#a, #b, a, b, req, __FILE__, __LINE__)
#define EXPECT_EQ(a, b) DTEST_CHECK(a, b, ::dtest::oper::eq, false)
#define EXPECT_NE(a, b) DTEST_CHECK(a, b, ::dtest::oper::neq, false)
#define EXPECT_LT(a, b) DTEST_CHECK(a, b, ::dtest::oper::lt, false)
#define EXPECT_LE(a, b) DTEST_CHECK(a, b, ::dtest::oper::le, false)
#define EXPECT_GT(a, b) DTEST_CHECK(a, b, ::dtest::oper::gt, false)
#define EXPECT_GE(a, b) DTEST_CHECK(a, b, ::dtest::oper::ge, false)
#define ASSERT_EQ(a, b) DTEST_CHECK(a, b, ::dtest::oper::eq, true)
#define ASSERT_NE(a, b) DTEST_CHECK(a, b, ::dtest::oper::neq, true)
#define ASSERT_LT(a, b) DTEST_CHECK(a, b, ::dtest::oper::lt, true)
#define ASSERT_LE(a, b) DTEST_CHECK(a, b, ::dtest::oper::le, true)
#define ASSERT_GT(a, b) DTEST_CHECK(a, b, ::dtest::oper::gt, true)
#define ASSERT_GE(a, b) DTEST_CHECK(a, b, ::dtest::oper::ge, true)

namespace dtest {
struct expr_t {
	std::string expr;
	std::string a;
	std::string b;
	oper op{};
	std::string_view file;
	std::uint64_t line{};
	bool pass{};
	bool required{};
};

struct atest_t {
	std::vector<expr_t> fails;
	std::string_view name;
	atest_t* next{};

	atest_t(std::string_view name) noexcept;

	virtual void operator()() = 0;

	void check(expr_t expr, bool required);
	template <oper Op, typename T, typename U>
	void check(std::string_view l, std::string_view r, T const& a, U const& b, bool required, std::string_view file, std::uint8_t line) {
		check({detail::expr(l, r, Op), detail::str(a), detail::str(b), Op, file, line, detail::operate<Op>(a, b), required}, required);
	}
};

int run_tests(bool async);
} // namespace dtest

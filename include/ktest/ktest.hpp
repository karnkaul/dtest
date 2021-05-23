#include <cstdint>
#include <string>
#include <vector>

///
/// \brief Usage: TEST(foo) { // test body... }
/// Create and register a new test
///
#define TEST(name)                                                                                                                                             \
	void test_##name(::ktest::helper_t&);                                                                                                                      \
	::ktest::test_t g_##name(#name, &test_##name);                                                                                                             \
	void test_##name([[maybe_unused]] ::ktest::helper_t& out_helper_)

#define KTEST_CHECK(a, b, op, req) out_helper_.check({#a " " #op " " #b, __FILE__, __LINE__, ((a)op(b))}, req)
#define EXPECT_EQ(a, b) KTEST_CHECK(a, b, ==, false)
#define EXPECT_NE(a, b) KTEST_CHECK(a, b, !=, false)
#define EXPECT_LT(a, b) KTEST_CHECK(a, b, <, false)
#define EXPECT_LE(a, b) KTEST_CHECK(a, b, <=, false)
#define EXPECT_GT(a, b) KTEST_CHECK(a, b, >, false)
#define EXPECT_GE(a, b) KTEST_CHECK(a, b, >=, false)
#define ASSERT_EQ(a, b) KTEST_CHECK(a, b, ==, true)
#define ASSERT_NE(a, b) KTEST_CHECK(a, b, !=, true)
#define ASSERT_LT(a, b) KTEST_CHECK(a, b, <, true)
#define ASSERT_LE(a, b) KTEST_CHECK(a, b, <=, true)
#define ASSERT_GT(a, b) KTEST_CHECK(a, b, >, true)
#define ASSERT_GE(a, b) KTEST_CHECK(a, b, >=, true)

namespace ktest {
struct expr_t {
	std::string_view str;
	std::string_view file;
	std::uint64_t line{};
	bool pass{};
	bool required{};
};

struct helper_t {
	std::vector<expr_t> fails;
	std::uint64_t passes;

	void check(expr_t const& expr, bool required);
};

using func_t = void (*)(helper_t&);

struct test_t {
	helper_t helper;
	std::string_view name;
	func_t func;

	test_t(std::string_view name, func_t func);
};

int run_tests(bool async);
} // namespace ktest

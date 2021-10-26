#pragma once
#include <sstream>
#include <string_view>
#include <type_traits>

namespace dtest {
enum class oper { eq, neq, lt, le, gt, ge };

namespace detail {
constexpr std::string_view g_ops[] = {"==", "!=", "<", "<=", ">", ">="};
constexpr std::string_view op_str(oper op) { return g_ops[static_cast<std::size_t>(op)]; }

inline std::string expr(std::string_view l, std::string_view r, oper op) {
	std::stringstream str;
	str << l << ' ' << op_str(op) << ' ' << r;
	return str.str();
}

template <oper>
constexpr bool false_v = false;

template <oper Op, typename T, typename U>
constexpr bool operate(T const& a, U const& b) noexcept {
	if constexpr (Op == oper::eq) {
		return a == b;
	} else if constexpr (Op == oper::neq) {
		return a != b;
	} else if constexpr (Op == oper::lt) {
		return a < b;
	} else if constexpr (Op == oper::le) {
		return a <= b;
	} else if constexpr (Op == oper::gt) {
		return a > b;
	} else if constexpr (Op == oper::ge) {
		return a >= b;
	} else {
		static_assert(false_v<Op>, "Invalid operator");
	}
}

template <typename T, typename = void>
struct is_streamable : std::false_type {};
template <typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T const&>())>> : std::true_type {};
template <typename T>
constexpr bool is_streamable_v = is_streamable<T>::value;

template <typename T>
std::string str(T const& t) {
	if constexpr (is_streamable_v<T>) {
		std::stringstream str;
		if constexpr (std::is_pointer_v<T>) {
			if (t == nullptr) {
				str << "nullptr";
			} else {
				str << t;
			}
		} else {
			str << t;
		}
		return str.str();
	} else {
		return {};
	}
}

inline std::string str(std::nullptr_t) { return "nullptr"; }
} // namespace detail
} // namespace dtest

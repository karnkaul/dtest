## dtest

Tiny test harness library in C++17.

## Features

1. Define multiple tests in single cpp files using `TEST(name) { // body ... }`
1. Perform checks using `EXPECT_X` and `ASSERT_X` macros
1. Multi-threaded execution of tests (can be disabled)
1. Consolidated report per test
1. Expression, filename, and line number per expectation / assertion failure
1. Configurable test fail return code (`1` by default)

## Limitations

1. No sub-tests / test cases / test groups
1. Global / per-test environment

## Usage

### Requirements

- CMake 3.3
- C++17 compiler (and stdlib)

### Steps

1. Clone repo to appropriate subdirectory, say `dtest`
1. Add library to project via: `add_subdirectory(dtest)`
1. To use just the test engine, link to `dtest::dtest`: `target_link_libraries(foo dtest::dtest)`, and in `main()` call `dtests::run_tests(bool async)`
1. To also use the built-in main function, link to `dtest::main`: `target_link_libraries(foo dtest::main)`
1. Use via: `#include <dumb_test/dtest.hpp>`

### Examples

```cmake
add_executable(foo dtest_demo.cpp)
target_link_libraries(foo PRIVATE dtest::main)
```

```cpp
// dtest_demo.cpp
#include <dumb_test/dtest.hpp>

namespace {
TEST(foo_test) {
  int const arr[] = {0, 1, 2};
  ASSERT_EQ(sizeof(arr) / sizeof(int), 3U); // stops further execution if assertion fails
  EXPECT_EQ(arr[0], 0);                     // records test failure and continues execution
}

struct f {
  int val{};
};
bool operator==(f a, f b) { return a.val == b.val; }

TEST(fail_test) {
  char const* x = "hi";
  std::string_view const y = "hello";
  EXPECT_EQ(x, y);  // prints values
  EXPECT_EQ(&x, nullptr); // prints address and "nullptr"
  f const a{99};
  f const b{-42};
  EXPECT_EQ(a, b);  // prints failed expression (a == b)
}
} // namespace
```

Output:

```
[======] starting 2 tests
[ pass ] foo_test
[ FAIL ] fail_test
        expected: hi == hello (dtest_demo.cpp | 18)
        expected: 0x7fc089637948 == nullptr (dtest_demo.cpp | 19)
        expected: a == b  (dtest_demo.cpp | 22)
[======] completed 2 tests (0.3ms)
[ pass ] 1 / 2
[ FAIL ] 1 / 2
```

### Configuration

These CMake variables can be modified for customization:

1. `DTEST_FAIL_RETURN_CODE`: Return code on test failure (`int`)
1. `DTEST_MULTITHREADED` : Whether `dtest::main` uses multiple threads (`bool`)

## Contributing

Pull/merge requests are welcome.

**[Original repository](https://github.com/karnkaul/dtest)**

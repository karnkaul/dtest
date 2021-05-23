## ktest

Tiny test harness library in C++17.

## Features

1. Define multiple tests in single cpp files using `TEST(name) { // body ... }`
1. Perform checks using `EXPECT_X` and `ASSERT_X` macros
1. Multi-threaded execution of tests (can be disabled)
1. Consolidated report per test
1. Expression, filename, and line number per expectation / assertion failure
1. Configurable test fail return code (`1` by default)

## Limitations

1. Max tests (configurable; this is because the backing store is a fixed-size array that's populated before `main()`)
1. No sub-tests / test cases / test groups
1. Global / per-test environment

## Usage

### Requirements

- CMake 3.3
- C++17 compiler (and stdlib)

### Steps

1. Clone repo to appropriate subdirectory, say `ktest`
1. Add library to project via: `add_subdirectory(ktest)`
1. To use just the test engine, link to `ktest::ktest`: `target_link_libraries(foo ktest::ktest)`, and in `main()` call `ktests::run_tests(bool async)`
1. To also use the built-in main function, link to `ktest::main`: `target_link_libraries(foo ktest::main)`
1. Use via: `#include <ktest/ktest.hpp>`

### Examples

```cmake
add_executable(foo test.cpp)
target_link_libraries(foo PRIVATE ktest::main)
```

```cpp
// test.cpp

TEST(foo_test) {
  int const arr[] = {0, 1, 2};
  ASSERT_EQ(sizeof(arr) / sizeof(int), 3); // stops further execution if assertion fails
  EXPECT_EQ(arr[0], 0); // records test failure and continues execution
}
```

Output:

```
[======] starting 1 tests
[ pass ] foo_test
[======] completed 1 tests (0.1ms)
[ pass ] 1 / 1
```

### Configuration

These CMake variables can be modified for customization:

1. `KTEST_MAX_TESTS`: Max number of tests that can be accommodated (default: `1024`)
1. `KTEST_FAIL_RETURN_CODE`: Return code on test failure (`int`)
1. `KTEST_MULTITHREADED` : Whether `ktest::main` uses multiple threads

## Contributing

Pull/merge requests are welcome.

**[Original repository](https://github.com/karnkaul/ktest)**

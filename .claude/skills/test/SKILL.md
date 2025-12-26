---
name: test
description: Run unit tests using CTest. Use when user asks to run tests, test the code, check test results, or mentions test failures.
allowed-tools: Bash, Read, Glob
---

# Test Skill

Run unit tests for the OrderParserProcessor project using CTest.

## Prerequisites

- Project must be built with `-DBUILD_TESTS=ON` option
- Tests are typically located in service directories under `test/` subdirectories (e.g., `backend/test/`, `connectivity/test/`)
- Identify the build directory (check for existing build directories)

## Instructions

1. Find the build directory (search for directories like `build/`, `cmake-build-*/`, `backend/build/`, etc.)
2. Check if tests were built (look for test executables or check CMake cache for BUILD_TESTS)
3. If tests not built, rebuild with tests: `cmake -B <build_dir> -DBUILD_TESTS=ON && cmake --build <build_dir>`
4. **Identify which specific tests to run** based on user request or context
5. Run the specific tests using CTest with pattern matching
6. Report test results (passed/failed counts)
7. If tests fail, show failure details and relevant error messages

## Test Commands

### Run Specific Tests (Recommended)
```bash
# Run tests matching a pattern
ctest --test-dir <build_dir> -R <test_name_pattern> --output-on-failure

# Examples:
ctest --test-dir <build_dir> -R backend --output-on-failure
ctest --test-dir <build_dir> -R connectivity --output-on-failure
ctest --test-dir <build_dir> -R script_processor --output-on-failure
```

### Run All Tests
```bash
ctest --test-dir <build_dir> --output-on-failure
```

### Run with Parallel Execution (Recommended for speed)
```bash
# Use -j2 or higher to run tests in parallel
# Some tests use timers, so parallel execution speeds things up
ctest --test-dir <build_dir> -j2 --output-on-failure

# Adjust based on available CPU cores
ctest --test-dir <build_dir> -j4 --output-on-failure
```

### List Available Tests
```bash
ctest --test-dir <build_dir> -N
```

### Run Tests Verbosely
```bash
ctest --test-dir <build_dir> -R <pattern> --verbose
```

### Rerun Failed Tests
```bash
ctest --test-dir <build_dir> --rerun-failed --output-on-failure
```

## Build Tests (if not already built)

```bash
cmake -B <build_dir> -DBUILD_TESTS=ON
cmake --build <build_dir>
```

## Test Organization

Tests are organized per service/component:
- `backend/test/` - Backend service tests
- `connectivity/test/` - Connectivity module tests
- Other component directories may have their own `test/` subdirectories

## Best Practices

1. **Run specific tests** rather than the entire suite when working on a particular component
2. **Use parallel execution** (`-j2` or higher) when running multiple tests - some tests use timers
3. **List tests first** with `-N` to see what's available
4. **Use pattern matching** with `-R` to target relevant tests

## Common Issues

- **No tests found**: Ensure `BUILD_TESTS=ON` was set during CMake configuration
- **Test compilation errors**: Check test source files in component `test/` directories
- **Test failures**: Review output with `--output-on-failure` flag
- **Build directory missing**: Run build first with tests enabled
- **Wrong build directory**: Search for actual build directory location
- **Tests taking too long**: Use `-j` for parallel execution or run specific tests with `-R`

## Success Criteria

- Requested tests pass successfully
- No segmentation faults or crashes
- Test output is clear and informative
- Targeted tests run efficiently

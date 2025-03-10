# Naive Numbers Testing Plan

This document outlines the testing strategy for the Naive Numbers library, with a particular focus on memory management and reference counting.

## Testing Goals

1. **Memory Management**: Ensure no memory leaks occur during normal operation
2. **Reference Counting**: Verify that reference counting works correctly for all number types
3. **Operation Chaining**: Test that complex chained operations handle memory correctly
4. **Edge Cases**: Test boundary conditions and error handling

## Test Categories

### 1. Unit Tests

Unit tests verify individual components function correctly in isolation.

#### Number Creation and Basic Operations
- Test creation of different number types (integer, float, double)
- Verify initial reference counts
- Test basic arithmetic operations

#### Vector Operations
- Test vector creation and manipulation
- Verify vector operations (addition, multiplication, etc.)
- Test vector map operations

#### Matrix Operations
- Test matrix creation and manipulation
- Verify matrix operations (transpose, multiplication, etc.)
- Test matrix decomposition algorithms

### 2. Memory Management Tests

These tests specifically target memory management.

#### Reference Counting
- Test incrementing and decrementing reference counts
- Verify objects are freed when reference count reaches zero
- Test NULL handling in reference counting functions

#### Chained Operations
- Test deeply nested operation chains
- Verify no memory leaks in complex expressions
- Test operations that modify objects in-place

### 3. Integration Tests

Integration tests verify that components work correctly together.

#### Probability Operations
- Test probability space creation and operations
- Verify statistical calculations (variance, covariance, etc.)
- Test conditional probability calculations

#### Complex Workflows
- Test realistic usage scenarios
- Verify end-to-end workflows

## Testing Tools

### Memory Leak Detection

We use the following tools to detect memory leaks:

1. **Valgrind**: For comprehensive memory analysis
   ```bash
   valgrind --leak-check=full ./test_program
   ```

2. **macOS leaks tool**: For quick leak detection on macOS
   ```bash
   leaks --atExit -- ./test_program
   ```

3. **Address Sanitizer**: For runtime memory error detection
   ```bash
   gcc -fsanitize=address -o test_program test_program.c
   ```

### Test Automation

All tests should be automated and run as part of the CI/CD pipeline. Tests should be:

1. **Repeatable**: Tests should produce the same results when run multiple times
2. **Independent**: Tests should not depend on each other
3. **Self-checking**: Tests should automatically verify their results

## Test Implementation

### Test Structure

Each test file should follow this structure:

1. **Setup**: Initialize test data
2. **Exercise**: Run the code being tested
3. **Verify**: Check the results
4. **Cleanup**: Free all allocated resources

### Test Assertions

Use the `test_assert` macro for consistent test assertions:

```c
test_assert(condition, "Error message if condition fails");
```

## Continuous Testing

Memory leak tests should be run:

1. After every significant code change
2. Before each release
3. Periodically on the main branch

## Debugging Memory Issues

When memory leaks are detected:

1. Identify the specific operation causing the leak
2. Check reference counting in the relevant functions
3. Verify that all code paths properly free resources
4. Add additional tests to cover the specific case

## Test Coverage

Aim for high test coverage, particularly for:

1. Memory allocation/deallocation code
2. Reference counting operations
3. Complex mathematical operations
4. Error handling paths

## Future Improvements

1. Add fuzzing tests to find edge cases
2. Implement performance benchmarks
3. Add stress tests for large data structures

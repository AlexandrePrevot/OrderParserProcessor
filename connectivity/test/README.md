# Connectivity Module Unit Tests

Unit tests for the connectivity module using Google Test and Google Mock.

## Structure

```
test/
├── mocks/                          # Mock implementations for testing
│   ├── mock_grpc_writer.h         # Mock gRPC ServerWriter
│   └── mock_server_context.h      # Mock gRPC ServerContext
├── unit/                           # Unit tests
│   ├── market_data_point_test.cc  # POD struct tests
│   ├── market_data_service_test.cc # Service layer tests
│   └── python_api_gtw_test.cc     # Gateway API tests
├── CMakeLists.txt                 # Test build configuration
└── README.md                      # This file
```

## Building Tests

Tests are built when the `BUILD_TESTS` CMake option is enabled:

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
```

## Running Tests

Run all connectivity tests:
```bash
ctest --test-dir build -R ConnectivityUnitTests
```

Run with verbose output:
```bash
ctest --test-dir build -R ConnectivityUnitTests --verbose
```

Run the test executable directly:
```bash
./build/connectivity/test/connectivity_tests
```

## Test Coverage

### market_data_point_test.cc
- ✅ Trivially copyable verification
- ✅ Default constructor initialization
- ✅ Instrument ID setting and truncation
- ✅ Null termination safety
- ✅ Copy constructor

### market_data_service_test.cc
- ✅ Service initialization
- ✅ StreamPrices method callable
- ✅ Null writer handling

### python_api_gtw_test.cc
- ✅ Gateway initialization
- ✅ Null writer handling
- ✅ Thread cleanup on destruction
- ⚠️  JSON parsing (requires mock socket or refactoring)
- ⚠️  Queue overflow behavior (requires mock socket or refactoring)

## Limitations

### Current Tests
The current tests verify:
- POD struct contracts (trivially copyable, size limits)
- API contracts (methods callable, null handling)
- Thread safety (no deadlocks on destruction)

### Missing Tests (Require Refactoring)
To fully test the JSON parsing and queue logic, `PythonApiGtw` would need refactoring to support dependency injection:

**Current design:**
```cpp
void PythonApiGtw::SocketReaderThread() {
  tcp::socket socket(ios);  // Hard-coded
  socket.connect(endpoint);  // Hard-coded
}
```

**Testable design:**
```cpp
// Abstract interface
class ISocketReader {
  virtual size_t ReadSome(char* buf, size_t size) = 0;
};

// Inject via constructor
PythonApiGtw::PythonApiGtw(std::unique_ptr<ISocketReader> reader);
```

With dependency injection, we could test:
- Valid JSON parsing
- Invalid JSON handling
- Queue overflow (drop messages)
- Partial JSON messages
- Connection failures
- Client disconnection

## Future Improvements

1. **Refactor for testability**: Add dependency injection for socket reader
2. **Add JSON parsing tests**: Test various JSON formats and edge cases
3. **Add queue tests**: Test overflow, concurrent access, performance
4. **Add threading tests**: Test concurrent reads/writes, race conditions
5. **Add benchmarks**: Measure throughput, latency, queue performance

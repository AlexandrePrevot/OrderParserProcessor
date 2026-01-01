# Google Test is fetched in main CMakeLists.txt

# currently most of the tests are useless
# in this component
# but at least the tests are setup

# Collect all unit test files
set(unit_tests
  unit/market_data_point_test.cc
  unit/market_data_service_test.cc
  unit/python_api_gtw_test.cc
)

# Create test executable
add_executable(connectivity_tests ${unit_tests})

# Include directories for mocks and main code
target_include_directories(connectivity_tests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_CURRENT_SOURCE_DIR}/../includes
)

# Link against main library and test frameworks
target_link_libraries(connectivity_tests
  PRIVATE
    lib_gateway
    lib_grpc_services
    lib_grpc_messages
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    gRPC::grpc++
    protobuf::libprotobuf
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(connectivity_tests)

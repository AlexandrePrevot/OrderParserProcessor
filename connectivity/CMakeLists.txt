cmake_minimum_required(VERSION 3.28.3)

project(BoostAsioExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# boost is necessary to ensure connection with the market
# with boost.ASIO
find_package(Boost 1.70 REQUIRED COMPONENTS system)

set(gateways_list
    python_api_gtw.cc
    services/market_data_service.cc
)

list(TRANSFORM gateways_list PREPEND "src/")

add_library(lib_gateway STATIC ${gateways_list})
target_include_directories(lib_gateway PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)
target_link_libraries(lib_gateway PUBLIC lib_grpc_services)
target_link_libraries(lib_gateway PUBLIC lib_grpc_messages)
target_link_libraries(lib_gateway PUBLIC nlohmann_json::nlohmann_json)


add_executable(distributor main.cc)

target_include_directories(lib_gateway PUBLIC ${Boost_INCLUDE_DIRS})
target_include_directories(lib_gateway PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(lib_gateway PUBLIC Boost::system)
target_link_libraries(distributor PUBLIC lib_gateway)
target_link_libraries(distributor PUBLIC gRPC::grpc++ protobuf::libprotobuf)

if (BUILD_TESTS)
  message("Connectivity tests will be built, because -DBUILD_TESTS=ON")
  add_subdirectory(test)
else()
  message("Connectivity tests will not be built, because -DBUILD_TESTS!=ON")
endif()

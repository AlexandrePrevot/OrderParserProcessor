cmake_minimum_required(VERSION 3.28.3)
project(OrderParserProcessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME} PUBLIC gRPC::grpc++ protobuf::libprotobuf)
cmake_minimum_required(VERSION 3.28.3)
project(OrderParserProcessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ANTLR_VERSION $ENV{ANTLR_VERSION})

option(BUILD_TESTS "Build unit tests" OFF)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_subdirectory(backend)

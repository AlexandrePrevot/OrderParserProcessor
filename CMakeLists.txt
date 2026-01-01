cmake_minimum_required(VERSION 3.28.3)
project(OrderParserProcessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ANTLR_VERSION $ENV{ANTLR_VERSION})

option(BUILD_TESTS "Build unit tests" OFF)

# ----------------------- Google Test (if tests enabled) -----------------------
if (BUILD_TESTS)
  include(FetchContent)

  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )

  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()
endif()
# ----------------------- END Google Test -----------------------

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# necessary to put here to include it
# to be included before adding subdirectory
# ----------------------- ANTLR -----------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include the ANTLR support
include(ExternalAntlr4Cpp)

# Path to the ANTLR tool (.jar)
set(ANTLR_EXECUTABLE /usr/local/lib/java/${ANTLR_VERSION}-complete.jar)

# Required for ANTLR integration
find_package(ANTLR REQUIRED)

# Generate parser + lexer + visitor
antlr_target(FiScriptGrammar
    ${CMAKE_SOURCE_DIR}/rules/parser/FiScript.g4
    VISITOR
    OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/generated/cpp/antlr/
)

include_directories(${ANTLR4_INCLUDE_DIRS})
include_directories(${ANTLR_FiScriptGrammar_OUTPUT_DIR})

# Produce a library from generated code
add_library(FiScriptGrammarLib
    ${ANTLR_FiScriptGrammar_CXX_OUTPUTS}
)
target_link_libraries(FiScriptGrammarLib PUBLIC antlr4_static)
# ----------------------- END ANTLR -----------------------

# ----------------------- protobuf and grpc -----------------------
file(GLOB grpc_services_list "${CMAKE_SOURCE_DIR}/generated/cpp/services/*.pb.cc")
add_library(lib_grpc_services STATIC ${grpc_services_list})
target_include_directories(lib_grpc_services PUBLIC ${CMAKE_SOURCE_DIR}/generated/cpp/services/)
target_include_directories(lib_grpc_services PUBLIC ${CMAKE_SOURCE_DIR}/generated/cpp/messages/)
target_include_directories(lib_grpc_services PUBLIC ${CMAKE_SOURCE_DIR}/generated/cpp/)
target_link_libraries(lib_grpc_services PUBLIC gRPC::grpc++ protobuf::libprotobuf)

file(GLOB grpc_messages_list "${CMAKE_SOURCE_DIR}/generated/cpp/messages/*.pb.cc")
add_library(lib_grpc_messages STATIC ${grpc_messages_list})
target_include_directories(lib_grpc_messages PUBLIC ${CMAKE_SOURCE_DIR}/generated/cpp/)
target_link_libraries(lib_grpc_messages PUBLIC gRPC::grpc++ protobuf::libprotobuf)

# ----------------------- END protobuf and grpc -----------------------

add_subdirectory(backend)
add_subdirectory(connectivity)

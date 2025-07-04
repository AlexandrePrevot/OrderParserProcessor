# ----------------------- protobuf and grpc -----------------------
# TODO : currently generated file dependencies is handled in here
# however if there are other cpp components, we should handle these dependencies
# in the root CMakeLists
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

# ----------------------- ANTLR -----------------------
add_definitions(-DANTLR4CPP_STATIC)


include(ExternalAntlr4Cpp)
include_directories(${ANTLR4_INCLUDE_DIRS})

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
set(ANTLR_EXECUTABLE /usr/local/lib/java/antlr-4.13.0-complete.jar)
find_package(ANTLR REQUIRED)

#add the output directories of the g4 files
#add_executable(output main.cpp)
#target_link_libraries(output antlr4_static)
# ----------------------- END ANTLR -----------------------


set(services_list
    script_submit_service.cc
)

list(TRANSFORM services_list PREPEND "src/services/")

set(processors_list
    script_submit_processor.cc
)

list(TRANSFORM processors_list PREPEND "src/processors/")

set(handlers_list
    script_submit_handler.cc
)

list(TRANSFORM handlers_list PREPEND "src/handlers/")


add_library(lib_services STATIC ${services_list})
target_include_directories(lib_services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)
target_link_libraries(lib_services PUBLIC lib_grpc_services)
target_link_libraries(lib_services PUBLIC lib_grpc_messages)

add_library(lib_processors STATIC ${processors_list})
target_include_directories(lib_processors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)
target_link_libraries(lib_processors PUBLIC lib_grpc_messages)

add_library(lib_handlers STATIC ${handlers_list})
target_include_directories(lib_handlers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)
target_link_libraries(lib_handlers PUBLIC lib_grpc_services)


add_executable(${PROJECT_NAME} main.cc)
target_link_libraries(${PROJECT_NAME} PUBLIC lib_handlers)
target_link_libraries(${PROJECT_NAME} PUBLIC lib_services)
target_link_libraries(${PROJECT_NAME} PUBLIC lib_processors)
target_link_libraries(${PROJECT_NAME} PUBLIC gRPC::grpc++ protobuf::libprotobuf)
target_link_libraries(${PROJECT_NAME} PUBLIC antlr4_static)

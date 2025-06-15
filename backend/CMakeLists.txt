cmake_minimum_required(VERSION 3.28.3)
project(OrderParserProcessor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

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

#services and processor should be available only to handler
add_library(lib_services STATIC ${services_list})
target_include_directories(lib_services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)
target_link_libraries(lib_services PUBLIC gRPC::grpc++ protobuf::libprotobuf) # protobuff probably would have to be include in
# other files aswell

add_library(lib_processors STATIC ${processors_list})
target_include_directories(lib_processors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)

add_library(lib_handlers STATIC ${handlers_list})
target_include_directories(lib_handlers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)


add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME} PUBLIC lib_services)
target_link_libraries(${PROJECT_NAME} PUBLIC lib_processors)

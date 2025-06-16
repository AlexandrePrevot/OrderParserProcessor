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

# include all generated cpp files
file(GLOB grpc_services_list "${CMAKE_SOURCE_DIR}/generated/cpp/services/*.pb.cc")


add_library(lib_grpc_services STATIC ${grpc_services_list})
target_link_libraries(lib_grpc_services PUBLIC gRPC::grpc++ protobuf::libprotobuf)
target_include_directories(lib_grpc_services PUBLIC ${CMAKE_SOURCE_DIR}/generated/cpp/services/)
target_include_directories(lib_grpc_services PUBLIC ${CMAKE_SOURCE_DIR}/generated/cpp/messages/)
target_include_directories(lib_grpc_services PUBLIC ${CMAKE_SOURCE_DIR}/generated/cpp/)

#services and processor should be available only to handler
add_library(lib_services STATIC ${services_list})
target_include_directories(lib_services PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)
target_link_libraries(lib_services PUBLIC lib_grpc_services)

# maybe should be PRIVATE ?
# protobuff probably would have to be include in
# other files aswell
target_link_libraries(lib_services PUBLIC gRPC::grpc++ protobuf::libprotobuf)

add_library(lib_processors STATIC ${processors_list})
target_include_directories(lib_processors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)

add_library(lib_handlers STATIC ${handlers_list})
target_include_directories(lib_handlers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)


add_executable(${PROJECT_NAME} main.cc)

target_link_libraries(${PROJECT_NAME} PUBLIC lib_services)
target_link_libraries(${PROJECT_NAME} PUBLIC lib_processors)

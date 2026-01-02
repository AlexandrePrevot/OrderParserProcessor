set(services_list
    script_submit_service.cc
    reacton_service.cc
)

list(TRANSFORM services_list PREPEND "src/services/")

set(processors_list
    script_submit_processor.cc
    common/timers.cc
    visitors/concrete_fiscript_visitor.cc
    visitors/file_maker.cc
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
target_link_libraries(lib_processors PUBLIC FiScriptGrammarLib)

add_library(lib_handlers STATIC ${handlers_list})
target_include_directories(lib_handlers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/)
target_link_libraries(lib_handlers PUBLIC lib_grpc_services)


add_executable(${PROJECT_NAME} main.cc)
target_link_libraries(${PROJECT_NAME} PUBLIC lib_handlers)
target_link_libraries(${PROJECT_NAME} PUBLIC lib_services)
target_link_libraries(${PROJECT_NAME} PUBLIC lib_processors)
target_link_libraries(${PROJECT_NAME} PUBLIC gRPC::grpc++ protobuf::libprotobuf)
target_link_libraries(${PROJECT_NAME} PUBLIC FiScriptGrammarLib)
#target_link_libraries(${PROJECT_NAME} PUBLIC antlr4_static)

if (BUILD_TESTS)
    message("tests will be build, because -DBUILD_TESTS=ON")
    add_subdirectory(test)
else()
    message("tests will not be build, because -DBUILD_TESTS!=ON")
endif()

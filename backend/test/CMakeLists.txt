# ----------- Google Test --------------
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# ----------- END Google Test --------------


enable_testing()


add_executable(
  processors_test
  processors/simple_parsing_test.cc 
  processors/common/timer_test.cc
  processors/visitors/file_maker_test.cc
)

target_include_directories(processors_test PUBLIC ${CMAKE_SOURCE_DIR}/backend/includes/)
target_link_libraries(processors_test PUBLIC lib_processors)
target_link_libraries(processors_test PUBLIC GTest::gtest_main)
target_link_libraries(processors_test PUBLIC FiScriptGrammarLib)

include(GoogleTest)
gtest_discover_tests(processors_test)
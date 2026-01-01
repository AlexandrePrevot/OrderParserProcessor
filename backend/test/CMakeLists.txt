# Google Test is fetched in main CMakeLists.txt

add_executable(
  processors_test
  processors/simple_parsing_test.cc 
  processors/common/timer_test.cc
  processors/visitors/concrete_fiscript_visitor_test.cc
  processors/visitors/file_maker_test.cc
)

target_include_directories(processors_test PUBLIC ${CMAKE_SOURCE_DIR}/backend/includes/)
target_link_libraries(processors_test PUBLIC lib_processors)
target_link_libraries(processors_test PUBLIC GTest::gtest_main)
target_link_libraries(processors_test PUBLIC FiScriptGrammarLib)

include(GoogleTest)
gtest_discover_tests(processors_test)
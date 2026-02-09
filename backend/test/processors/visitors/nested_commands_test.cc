#include <gtest/gtest.h>
#include <iostream>

#include "processors/visitors/concrete_fiscript_visitor.h"
#include "processors/visitors/file_maker.h"

TEST(NestedCommandsTest, ReactOnInsideSchedule) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "Schedule(Start, 3s, 3) {\n"
    "    ReactOn(\"AAPL\", 5) {\n"
    "        Print(\"Nested!\")\n"
    "    }\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for ReactOn inside Schedule:\n" << generated << std::endl;

  EXPECT_NE(generated.find("timer_manager"), std::string::npos);
  EXPECT_NE(generated.find("reacton_service"), std::string::npos);
  EXPECT_NE(generated.find("CreateTimer([=, &reacton_service]()"),
            std::string::npos);
  EXPECT_NE(generated.find("RegisterReaction(\"AAPL\", 5, [=, &reacton_service](const internal::PriceUpdate &quote)"),
            std::string::npos);
}

TEST(NestedCommandsTest, ScheduleInsideReactOn) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "ReactOn(\"AAPL\", 5) {\n"
    "    Schedule(Start, 2s, 1) {\n"
    "        Print(\"Nested timer!\")\n"
    "    }\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for Schedule inside ReactOn:\n" << generated << std::endl;

  EXPECT_NE(generated.find("timer_manager"), std::string::npos);
  EXPECT_NE(generated.find("reacton_service"), std::string::npos);
  EXPECT_NE(generated.find("RegisterReaction(\"AAPL\", 5, [=, &timer_manager](const internal::PriceUpdate &quote)"),
            std::string::npos);
  EXPECT_NE(generated.find("CreateTimer([=, &timer_manager]()"), std::string::npos);
}

TEST(NestedCommandsTest, OnlyScheduleNoNesting) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "Schedule(Start, 3s, 3) {\n"
    "    Print(\"Simple\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for simple Schedule:\n" << generated << std::endl;

  EXPECT_NE(generated.find("timer_manager"), std::string::npos);
  EXPECT_EQ(generated.find("reacton_service"), std::string::npos);
  EXPECT_NE(generated.find("CreateTimer([=]()"), std::string::npos);
}

TEST(NestedCommandsTest, ReactOnInsideReactOn) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "ReactOn(\"AAPL\", 5) {\n"
    "    Print(\"outer\")\n"
    "    ReactOn(\"MSFT\", 3) {\n"
    "        Print(\"inner\")\n"
    "    }\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for ReactOn inside ReactOn:\n" << generated << std::endl;

  EXPECT_NE(generated.find("reacton_service"), std::string::npos);
  EXPECT_NE(generated.find("RegisterReaction(\"AAPL\", 5, [=, &reacton_service](const internal::PriceUpdate &quote)"),
            std::string::npos);
  EXPECT_NE(generated.find("RegisterReaction(\"MSFT\", 3, [=, &reacton_service](const internal::PriceUpdate &quote)"),
            std::string::npos);
}

TEST(NestedCommandsTest, MultipleScheduleAndReactOnAtSameLevel) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "Schedule(Start, 3s, 3) {\n"
    "    Print(\"first schedule\")\n"
    "}\n"
    "ReactOn(\"AAPL\", 5) {\n"
    "    Print(\"first reacton\")\n"
    "}\n"
    "Schedule(Start, 2s, 2) {\n"
    "    Print(\"second schedule\")\n"
    "}\n"
    "ReactOn(\"MSFT\", 3) {\n"
    "    Print(\"second reacton\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(4, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for multiple Schedule and ReactOn at same level:\n" << generated << std::endl;

  EXPECT_NE(generated.find("timer_manager"), std::string::npos);
  EXPECT_NE(generated.find("reacton_service"), std::string::npos);

  size_t first_create = generated.find("CreateTimer([=]()");
  EXPECT_NE(first_create, std::string::npos);

  size_t second_create = generated.find("CreateTimer([=]()");
  EXPECT_NE(second_create, std::string::npos);

  size_t first_register = generated.find("RegisterReaction(\"AAPL\", 5, [=](const internal::PriceUpdate &quote)");
  EXPECT_NE(first_register, std::string::npos);

  size_t second_register = generated.find("RegisterReaction(\"MSFT\", 3, [=](const internal::PriceUpdate &quote)");
  EXPECT_NE(second_register, std::string::npos);
  EXPECT_GT(second_register, first_register);
}

TEST(NestedCommandsTest, ComplexNestedWithScheduleAndReactOn) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "ReactOn(\"AAPL\", 5) {\n"
    "    Print(\"lol\")\n"
    "    Schedule(Start, 3s, 3) {\n"
    "        Print(\"hello world !\")\n"
    "    }\n"
    "    ReactOn(\"AAPL\", 3) {\n"
    "        Print(\"wouhouuuu\")\n"
    "    }\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for complex nested Schedule and ReactOn:\n" << generated << std::endl;

  EXPECT_NE(generated.find("timer_manager"), std::string::npos);
  EXPECT_NE(generated.find("reacton_service"), std::string::npos);

  EXPECT_NE(generated.find("RegisterReaction(\"AAPL\", 5, [=, &timer_manager, &reacton_service](const internal::PriceUpdate &quote)"),
            std::string::npos);
  EXPECT_NE(generated.find("CreateTimer([=, &timer_manager, &reacton_service]()"), std::string::npos);
  EXPECT_NE(generated.find("RegisterReaction(\"AAPL\", 3, [=, &timer_manager, &reacton_service](const internal::PriceUpdate &quote)"),
            std::string::npos);
}

TEST(NestedCommandsTest, FullScriptFromFile) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "Print(\"Heyy !!\")\n"
    "myStr = \"str\" + 3 + (\"3\" + 4)\n"
    "ReactOn(\"AAPL\", 5) {\n"
    "    Print(\"lol\")\n"
    "    Schedule(Start, 3s, 3) {\n"
    "        Print(\"hello world !\")\n"
    "    }\n"
    "    ReactOn(\"AAPL\", 3) {\n"
    "        Print(\"wouhouuuu\")\n"
    "    }\n"
    "}\n"
    "Schedule(Start, 3s, 3) {\n"
    "    Print(\"hello world !\")\n"
    "}\n"
    "ReactOn(\"AAPL\", 3) {\n"
    "    Print(\"wouhouuuu\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(5, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for full script from file:\n" << generated << std::endl;

  EXPECT_NE(generated.find("timer_manager"), std::string::npos);
  EXPECT_NE(generated.find("reacton_service"), std::string::npos);
  EXPECT_NE(generated.find("std::string myStr"), std::string::npos);

  EXPECT_NE(generated.find("std::cout << (std::string(\"Heyy !!\"))"), std::string::npos);

  size_t nested_reacton_outer = generated.find("RegisterReaction(\"AAPL\", 5, [=, &timer_manager, &reacton_service](const internal::PriceUpdate &quote)");
  EXPECT_NE(nested_reacton_outer, std::string::npos);

  size_t nested_schedule = generated.find("CreateTimer([=, &timer_manager, &reacton_service]()");
  EXPECT_NE(nested_schedule, std::string::npos);
  EXPECT_GT(nested_schedule, nested_reacton_outer);

  size_t nested_reacton_inner = generated.find("RegisterReaction(\"AAPL\", 3, [=, &timer_manager, &reacton_service](const internal::PriceUpdate &quote)");
  EXPECT_NE(nested_reacton_inner, std::string::npos);

  size_t standalone_schedule = generated.find("CreateTimer([=, &timer_manager, &reacton_service]()");
  EXPECT_NE(standalone_schedule, std::string::npos);

  size_t standalone_reacton = generated.rfind("RegisterReaction(\"AAPL\", 3, [=, &timer_manager, &reacton_service](const internal::PriceUpdate &quote)");
  EXPECT_NE(standalone_reacton, std::string::npos);
}

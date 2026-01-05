#include <gtest/gtest.h>
#include <iostream>

#include "processors/visitors/concrete_fiscript_visitor.h"

constexpr std::string_view kScheduleThenPrint =
    "Schedule(Start, 5s, 4) { Print(\"Just a test!\") }\n"
    "Print(\"Just another test!\")\n";

TEST(ConcreteFileScriptVisitorTest, CheckCommand) {
  ConcreteFiScriptVisitor visitor;

  EXPECT_TRUE(visitor.Compile(std::string(kScheduleThenPrint)));

  const auto &command_list = visitor.get_commands_list();

  EXPECT_EQ(2, command_list.size());

  Command command1 = command_list[0];
  Command command2 = command_list[1];

  ASSERT_EQ(3, command1.arguments.size());
  ASSERT_EQ(1, command2.arguments.size());

  EXPECT_EQ(Command::CommandType::Schedule, command1.type);
  EXPECT_EQ("Start", command1.arguments[0]);
  EXPECT_EQ("5s", command1.arguments[1]);
  EXPECT_EQ("4", command1.arguments[2]);

  EXPECT_EQ(Command::CommandType::Print, command2.type);
  EXPECT_EQ("\"Just another test!\"", command2.arguments[0]);

  ASSERT_EQ(1, command1.in_scope.size());

  Command command3 = command1.in_scope[0];

  ASSERT_EQ(1, command3.arguments.size());
  EXPECT_EQ(Command::CommandType::Print, command3.type);
  EXPECT_EQ("\"Just a test!\"", command3.arguments[0]);
}
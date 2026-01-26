#include <gtest/gtest.h>
#include <iostream>

#include "processors/visitors/concrete_fiscript_visitor.h"
#include "processors/visitors/file_maker.h"

TEST(IfStatementTest, SimpleIfStatement) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 10\n"
    "if (x > 5) {\n"
    "    Print(\"x is greater than 5\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());
  EXPECT_EQ(Command::CommandType::VariableDeclaration, commands[0].type);
  EXPECT_EQ(Command::CommandType::If, commands[1].type);

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for simple if:\n" << generated << std::endl;

  EXPECT_NE(generated.find("double x = 10"), std::string::npos);
  EXPECT_NE(generated.find("if (x > 5) {"), std::string::npos);
  EXPECT_NE(generated.find("std::cout << (std::string(\"x is greater than 5\"))"), std::string::npos);
  EXPECT_NE(generated.find("}"), std::string::npos);
  EXPECT_EQ(generated.find("else"), std::string::npos);
}

TEST(IfStatementTest, IfWithElse) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 3\n"
    "if (x > 5) {\n"
    "    Print(\"x is greater than 5\")\n"
    "} else {\n"
    "    Print(\"x is 5 or less\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());
  EXPECT_EQ(Command::CommandType::If, commands[1].type);

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for if with else:\n" << generated << std::endl;

  EXPECT_NE(generated.find("if (x > 5) {"), std::string::npos);
  EXPECT_NE(generated.find("} else {"), std::string::npos);
  EXPECT_NE(generated.find("std::cout << (std::string(\"x is greater than 5\"))"), std::string::npos);
  EXPECT_NE(generated.find("std::cout << (std::string(\"x is 5 or less\"))"), std::string::npos);
}

TEST(IfStatementTest, IfWithElseIf) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 7\n"
    "if (x > 10) {\n"
    "    Print(\"x is greater than 10\")\n"
    "} else if (x > 5) {\n"
    "    Print(\"x is greater than 5 but not greater than 10\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());
  EXPECT_EQ(Command::CommandType::If, commands[1].type);

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for if with else if:\n" << generated << std::endl;

  EXPECT_NE(generated.find("if (x > 10) {"), std::string::npos);
  EXPECT_NE(generated.find("} else if (x > 5) {"), std::string::npos);
  EXPECT_NE(generated.find("std::cout << (std::string(\"x is greater than 10\"))"), std::string::npos);
  EXPECT_NE(generated.find("std::cout << (std::string(\"x is greater than 5 but not greater than 10\"))"), std::string::npos);
}

TEST(IfStatementTest, IfWithMultipleElseIf) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 7\n"
    "if (x > 15) {\n"
    "    Print(\"x > 15\")\n"
    "} else if (x > 10) {\n"
    "    Print(\"x > 10\")\n"
    "} else if (x > 5) {\n"
    "    Print(\"x > 5\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());
  EXPECT_EQ(Command::CommandType::If, commands[1].type);

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for if with multiple else if:\n" << generated << std::endl;

  EXPECT_NE(generated.find("if (x > 15) {"), std::string::npos);
  EXPECT_NE(generated.find("} else if (x > 10) {"), std::string::npos);
  EXPECT_NE(generated.find("} else if (x > 5) {"), std::string::npos);
}

TEST(IfStatementTest, CompleteIfElseIfElse) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 3\n"
    "if (x > 15) {\n"
    "    Print(\"x > 15\")\n"
    "} else if (x > 10) {\n"
    "    Print(\"x > 10\")\n"
    "} else if (x > 5) {\n"
    "    Print(\"x > 5\")\n"
    "} else {\n"
    "    Print(\"x <= 5\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());
  EXPECT_EQ(Command::CommandType::If, commands[1].type);

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for complete if/else if/else:\n" << generated << std::endl;

  EXPECT_NE(generated.find("if (x > 15) {"), std::string::npos);
  EXPECT_NE(generated.find("} else if (x > 10) {"), std::string::npos);
  EXPECT_NE(generated.find("} else if (x > 5) {"), std::string::npos);
  EXPECT_NE(generated.find("} else {"), std::string::npos);
  EXPECT_NE(generated.find("std::cout << (std::string(\"x <= 5\"))"), std::string::npos);
}

TEST(IfStatementTest, IfWithBooleanVariable) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "isReady = True\n"
    "if (isReady) {\n"
    "    Print(\"Ready!\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for if with boolean variable:\n" << generated << std::endl;

  EXPECT_NE(generated.find("bool isReady = true"), std::string::npos);
  EXPECT_NE(generated.find("if (isReady) {"), std::string::npos);
}

TEST(IfStatementTest, IfWithLogicalOperators) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 10\n"
    "y = 20\n"
    "if (x > 5 and y < 25) {\n"
    "    Print(\"Both conditions true\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(3, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for if with logical operators:\n" << generated << std::endl;

  EXPECT_NE(generated.find("if (x > 5 && y < 25) {"), std::string::npos);
}

TEST(IfStatementTest, IfWithEquality) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 10\n"
    "if (x == 10) {\n"
    "    Print(\"x equals 10\")\n"
    "} else {\n"
    "    Print(\"x does not equal 10\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for if with equality:\n" << generated << std::endl;

  EXPECT_NE(generated.find("if (x == 10) {"), std::string::npos);
}

TEST(IfStatementTest, NestedIfStatements) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 10\n"
    "if (x > 5) {\n"
    "    if (x > 8) {\n"
    "        Print(\"x > 8\")\n"
    "    }\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());
  EXPECT_EQ(Command::CommandType::If, commands[1].type);
  ASSERT_EQ(1, commands[1].in_scope.size());
  EXPECT_EQ(Command::CommandType::If, commands[1].in_scope[0].type);

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for nested if:\n" << generated << std::endl;

  EXPECT_NE(generated.find("if (x > 5) {"), std::string::npos);
  EXPECT_NE(generated.find("if (x > 8) {"), std::string::npos);
}

TEST(IfStatementTest, IfInsideSchedule) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "Schedule(Start, 3s, 3) {\n"
    "    x = 10\n"
    "    if (x > 5) {\n"
    "        Print(\"x > 5 inside schedule\")\n"
    "    }\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());
  EXPECT_EQ(Command::CommandType::Schedule, commands[0].type);
  ASSERT_EQ(2, commands[0].in_scope.size());
  EXPECT_EQ(Command::CommandType::VariableDeclaration, commands[0].in_scope[0].type);
  EXPECT_EQ(Command::CommandType::If, commands[0].in_scope[1].type);

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for if inside Schedule:\n" << generated << std::endl;

  EXPECT_NE(generated.find("timer_manager.CreateTimer"), std::string::npos);
  EXPECT_NE(generated.find("double x = 10"), std::string::npos);
  EXPECT_NE(generated.find("if (x > 5) {"), std::string::npos);
}

TEST(IfStatementTest, ScheduleInsideIf) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 10\n"
    "if (x > 5) {\n"
    "    Schedule(Start, 3s, 3) {\n"
    "        Print(\"Schedule inside if\")\n"
    "    }\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());
  EXPECT_EQ(Command::CommandType::If, commands[1].type);
  ASSERT_EQ(1, commands[1].in_scope.size());
  EXPECT_EQ(Command::CommandType::Schedule, commands[1].in_scope[0].type);

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for Schedule inside if:\n" << generated << std::endl;

  EXPECT_NE(generated.find("if (x > 5) {"), std::string::npos);
  EXPECT_NE(generated.find("timer_manager.CreateTimer"), std::string::npos);
}

TEST(IfStatementTest, IfInsideReactOn) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "ReactOn(\"AAPL\", 5) {\n"
    "    x = 10\n"
    "    if (x == 10) {\n"
    "        Print(\"x equals 10\")\n"
    "    } else {\n"
    "        Print(\"x does not equal 10\")\n"
    "    }\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());
  EXPECT_EQ(Command::CommandType::ReactOn, commands[0].type);
  ASSERT_EQ(2, commands[0].in_scope.size());
  EXPECT_EQ(Command::CommandType::If, commands[0].in_scope[1].type);

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for if inside ReactOn:\n" << generated << std::endl;

  EXPECT_NE(generated.find("reacton_service.RegisterReaction"), std::string::npos);
  EXPECT_NE(generated.find("if (x == 10) {"), std::string::npos);
  EXPECT_NE(generated.find("} else {"), std::string::npos);
}

TEST(IfStatementTest, MultipleStatementsInIfBlock) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 10\n"
    "if (x > 5) {\n"
    "    y = x + 5\n"
    "    Print(\"y = \")\n"
    "    z = y * 2\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());
  EXPECT_EQ(Command::CommandType::If, commands[1].type);
  ASSERT_EQ(3, commands[1].in_scope.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for multiple statements in if:\n" << generated << std::endl;

  EXPECT_NE(generated.find("if (x > 5) {"), std::string::npos);
  EXPECT_NE(generated.find("double y = x + 5"), std::string::npos);
  EXPECT_NE(generated.find("double z = y * 2"), std::string::npos);
}

TEST(IfStatementTest, ComplexNestedWithScheduleAndIf) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "Schedule(Start, 3s, 3) {\n"
    "    x = 10\n"
    "    if (x > 5) {\n"
    "        Print(\"x > 5\")\n"
    "        if (x > 8) {\n"
    "            Print(\"x > 8\")\n"
    "        }\n"
    "    } else {\n"
    "        Print(\"x <= 5\")\n"
    "    }\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for complex nested with Schedule and if:\n" << generated << std::endl;

  EXPECT_NE(generated.find("timer_manager.CreateTimer"), std::string::npos);
  EXPECT_NE(generated.find("if (x > 5) {"), std::string::npos);
  EXPECT_NE(generated.find("if (x > 8) {"), std::string::npos);
  EXPECT_NE(generated.find("} else {"), std::string::npos);
}

TEST(IfStatementTest, VariableScoping) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 10\n"
    "if (x > 5) {\n"
    "    y = 20\n"
    "    Print(\"y inside if\")\n"
    "}\n"
    "if (x < 15) {\n"
    "    y = 30\n"
    "    Print(\"y inside second if\")\n"
    "}\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(3, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code for variable scoping:\n" << generated << std::endl;

  size_t first_y = generated.find("double y = 20");
  EXPECT_NE(first_y, std::string::npos);

  size_t second_y = generated.find("double y = 30");
  EXPECT_NE(second_y, std::string::npos);
}

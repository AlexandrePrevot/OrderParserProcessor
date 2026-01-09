#include <gtest/gtest.h>
#include "processors/visitors/concrete_fiscript_visitor.h"
#include "processors/visitors/file_maker.h"

TEST(VariableExpressionTest, SimpleStringPlusNumber) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile("myString = \"str\" + 3 + 4\n"));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  EXPECT_EQ(Command::CommandType::VariableDeclaration, commands[0].type);
  EXPECT_EQ("myString", commands[0].variable_name);
  EXPECT_NE(nullptr, commands[0].expression.get());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
  std::string code = maker.GetCode();
  EXPECT_NE(code.find("std::string myString"), std::string::npos);
  EXPECT_NE(code.find("std::to_string(3)"), std::string::npos);
  EXPECT_NE(code.find("std::to_string(4)"), std::string::npos);
}

TEST(VariableExpressionTest, StringPlusParentheses) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile("myString = \"str\" + (3 + 4)\n"));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
  std::string code = maker.GetCode();
  EXPECT_NE(code.find("std::string myString"), std::string::npos);
}

TEST(VariableExpressionTest, StringPlusStringPlusNumber) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile("myString = \"str\" + (\"thing\" + 4)\n"));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
  std::string code = maker.GetCode();
  EXPECT_NE(code.find("std::string myString"), std::string::npos);
}

TEST(VariableExpressionTest, ComplexNested) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile(
    "temp = \"temp\"\n"
    "myStr = \"str\" + 3 + (temp + 4 + (5 + 10))\n"
  ));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
  std::string code = maker.GetCode();
  std::cout << "Generated code for complex nested:\n" << code << std::endl;
  EXPECT_NE(code.find("std::string temp"), std::string::npos);
  EXPECT_NE(code.find("std::string myStr"), std::string::npos);
}

TEST(VariableExpressionTest, StringConcatenation) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile("myString = \"str\" + 3 + 3\n"));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
}

TEST(VariableExpressionTest, StringWithEvaluation) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile("myString = \"str\" + (3 + 3)\n"));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());

  std::string code = maker.GetCode();
  std::cout << "Generated code for myString = \"str\" + (3 + 3):\n" << code << std::endl;
  EXPECT_NE(code.find("std::to_string(3 + 3)"), std::string::npos);
  EXPECT_EQ(code.find("std::to_string(3) + std::to_string(3)"), std::string::npos);
}

TEST(VariableExpressionTest, StringWithMultiplication) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile("myStr = \"str\" + 3 + 4 * 2\n"));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
}

TEST(VariableExpressionTest, CompoundAssignmentNumeric) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile(
    "myNum = 5\n"
    "myNum += 3\n"
  ));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
  std::string code = maker.GetCode();
  EXPECT_NE(code.find("double myNum"), std::string::npos);
  EXPECT_NE(code.find("myNum += "), std::string::npos);
}

TEST(VariableExpressionTest, CompoundAssignmentString) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile(
    "myString = \"hello\"\n"
    "myString += 3\n"
  ));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
  std::string code = maker.GetCode();
  EXPECT_NE(code.find("std::string myString"), std::string::npos);
  EXPECT_NE(code.find("myString += "), std::string::npos);
}

TEST(VariableExpressionTest, PureNumericExpression) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile("result = 5 + 3 * 2\n"));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
  std::string code = maker.GetCode();
  EXPECT_NE(code.find("double result"), std::string::npos);
}

TEST(VariableExpressionTest, VariableReference) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile(
    "a = 10\n"
    "b = a * 2\n"
  ));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
  std::string code = maker.GetCode();
  EXPECT_NE(code.find("double a"), std::string::npos);
  EXPECT_NE(code.find("double b"), std::string::npos);
}

TEST(VariableExpressionTest, RedeclarationWithoutType) {
  ConcreteFiScriptVisitor visitor;
  EXPECT_TRUE(visitor.Compile(
    "myStr = \"str\" + 3 + (\"3\" + 4)\n"
    "myStr = \"lolilol\" + 3 + (\"3\" + 4)\n"
  ));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());

  FileMaker maker(commands);
  EXPECT_TRUE(maker.Compiled());
  std::string code = maker.GetCode();
  std::cout << "Generated code for redeclaration:\n" << code << std::endl;

  size_t first_decl = code.find("std::string myStr");
  size_t second_decl = code.find("std::string myStr", first_decl + 1);

  EXPECT_NE(first_decl, std::string::npos);
  EXPECT_EQ(second_decl, std::string::npos);

  size_t first_include = code.find("#include <string>");
  size_t second_include = code.find("#include <string>", first_include + 1);

  EXPECT_NE(first_include, std::string::npos);
  EXPECT_EQ(second_include, std::string::npos);
}

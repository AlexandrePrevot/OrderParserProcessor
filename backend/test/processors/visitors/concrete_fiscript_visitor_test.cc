#include <gtest/gtest.h>
#include <iostream>

#include "processors/visitors/concrete_fiscript_visitor.h"

constexpr std::string_view kScheduleThenPrint =
    "Schedule(Start, 5s, 4) { Print(\"Just a test!\") }\n"
    "Print(\"Just another test!\")\n";

constexpr std::string_view kPrintWithExpression =
    "myStr = \"test\"\n"
    "Print(myStr + \" value\")\n";

TEST(ConcreteFileScriptVisitorTest, CheckCommand) {
  ConcreteFiScriptVisitor visitor;

  EXPECT_TRUE(visitor.Compile(std::string(kScheduleThenPrint)));

  const auto &command_list = visitor.get_commands_list();

  EXPECT_EQ(2, command_list.size());

  Command command1 = command_list[0];
  Command command2 = command_list[1];

  ASSERT_EQ(3, command1.arguments.size());
  EXPECT_EQ(Command::CommandType::Schedule, command1.type);
  EXPECT_EQ("Start", command1.arguments[0]);
  EXPECT_EQ("5s", command1.arguments[1]);
  EXPECT_EQ("4", command1.arguments[2]);

  EXPECT_EQ(Command::CommandType::Print, command2.type);
  ASSERT_NE(command2.expression, nullptr);
  EXPECT_EQ(ExprNode::Type::Literal, command2.expression->node_type);
  const auto* lit2 = static_cast<const LiteralNode*>(command2.expression.get());
  EXPECT_TRUE(lit2->is_string);
  EXPECT_EQ("\"Just another test!\"", lit2->value);

  ASSERT_EQ(1, command1.in_scope.size());

  Command command3 = command1.in_scope[0];

  EXPECT_EQ(Command::CommandType::Print, command3.type);
  ASSERT_NE(command3.expression, nullptr);
  EXPECT_EQ(ExprNode::Type::Literal, command3.expression->node_type);
  const auto* lit3 = static_cast<const LiteralNode*>(command3.expression.get());
  EXPECT_TRUE(lit3->is_string);
  EXPECT_EQ("\"Just a test!\"", lit3->value);
}

TEST(ConcreteFileScriptVisitorTest, PrintWithExpression) {
  ConcreteFiScriptVisitor visitor;

  EXPECT_TRUE(visitor.Compile(std::string(kPrintWithExpression)));

  const auto &command_list = visitor.get_commands_list();
  ASSERT_EQ(2, command_list.size());

  // First command: variable declaration
  Command command1 = command_list[0];
  EXPECT_EQ(Command::CommandType::VariableDeclaration, command1.type);
  EXPECT_EQ("myStr", command1.variable_name);
  ASSERT_NE(command1.expression, nullptr);

  // Second command: Print with expression
  Command command2 = command_list[1];
  EXPECT_EQ(Command::CommandType::Print, command2.type);
  ASSERT_NE(command2.expression, nullptr);
  EXPECT_EQ(ExprNode::Type::BinaryOp, command2.expression->node_type);

  const auto* binop = static_cast<const BinaryOpNode*>(command2.expression.get());
  EXPECT_EQ("+", binop->op);

  // Left side should be variable reference
  ASSERT_NE(binop->left, nullptr);
  EXPECT_EQ(ExprNode::Type::VariableRef, binop->left->node_type);
  const auto* varRef = static_cast<const VariableRefNode*>(binop->left.get());
  EXPECT_EQ("myStr", varRef->var_name);

  // Right side should be string literal
  ASSERT_NE(binop->right, nullptr);
  EXPECT_EQ(ExprNode::Type::Literal, binop->right->node_type);
  const auto* lit = static_cast<const LiteralNode*>(binop->right.get());
  EXPECT_TRUE(lit->is_string);
  EXPECT_EQ("\" value\"", lit->value);
}
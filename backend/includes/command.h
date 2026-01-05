#pragma once

#include <string>
#include <vector>
#include <memory>

enum class VariableType { Numeric, String };

struct ExprNode {
  enum class Type { Literal, VariableRef, BinaryOp, Paren };

  Type node_type;
  virtual ~ExprNode() = default;
};

struct LiteralNode : ExprNode {
  std::string value;
  bool is_string;

  LiteralNode(const std::string& val, bool str) : value(val), is_string(str) {
    node_type = Type::Literal;
  }
};

struct VariableRefNode : ExprNode {
  std::string var_name;

  VariableRefNode(const std::string& name) : var_name(name) {
    node_type = Type::VariableRef;
  }
};

struct BinaryOpNode : ExprNode {
  std::string op;
  std::shared_ptr<ExprNode> left;
  std::shared_ptr<ExprNode> right;

  BinaryOpNode(const std::string& operation,
               std::shared_ptr<ExprNode> l,
               std::shared_ptr<ExprNode> r)
      : op(operation), left(std::move(l)), right(std::move(r)) {
    node_type = Type::BinaryOp;
  }
};

struct ParenExprNode : ExprNode {
  std::shared_ptr<ExprNode> inner;

  ParenExprNode(std::shared_ptr<ExprNode> expr) : inner(std::move(expr)) {
    node_type = Type::Paren;
  }
};

struct Command {
  enum CommandType { Schedule, ReactOn, Print, VariableDeclaration, VariableAssignment };

  CommandType type;
  std::vector<std::string> arguments;
  std::vector<Command> in_scope;

  std::string variable_name;
  std::string value_expr;
  std::string compound_op;
  VariableType var_type;

  std::shared_ptr<ExprNode> expression;
};
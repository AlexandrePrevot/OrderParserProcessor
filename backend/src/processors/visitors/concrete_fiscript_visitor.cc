#include "processors/visitors/concrete_fiscript_visitor.h"
#include "antlr/FiScriptLexer.h"
#include "antlr/FiScriptParser.h"

#include <iostream>

bool ConcreteFiScriptVisitor::Compile(const std::string &code) {
  antlr4::ANTLRInputStream input(code);
  FiScriptLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  FiScriptParser parser(&tokens);

  parser.setErrorHandler(std::make_shared<antlr4::BailErrorStrategy>());

  FiScriptParser::ScriptContext *tree;

  try {
    tree = parser.script();
  } catch (antlr4::ParseCancellationException &e) {
    // will not display anything, check
    // https://stackoverflow.com/questions/56205952/how-to-catch-parse-exceptions-in-antlr4-using-c
    // to see how to handle errors
    std::cout << "could not compile input reason : \n" << e.what() << std::endl;
    return false;
  }

  visitScript(tree);

  return true;
}

std::any
ConcreteFiScriptVisitor::visitScript(FiScriptParser::ScriptContext *ctx) {
  for (const auto &statement : ctx->statement()) {
    commands_list_.push_back(std::any_cast<Command>(visitStatement(statement)));
  }

  return nullptr;
}

std::any
ConcreteFiScriptVisitor::visitStatement(FiScriptParser::StatementContext *ctx) {
  FiScriptParser::ReactonContext *reacton = ctx->reacton();
  FiScriptParser::ScheduleContext *schedule = ctx->schedule();
  FiScriptParser::PrintContext *print = ctx->print();
  FiScriptParser::AlertContext *alert = ctx->alert();
  FiScriptParser::IfContext *ifStmt = ctx->if_();
  FiScriptParser::VariableDeclarationContext *varDecl = ctx->variableDeclaration();
  FiScriptParser::VariableAssignmentContext *varAssign = ctx->variableAssignment();

  if (alert != nullptr) {
    return visitAlert(alert);
  } else if (reacton != nullptr) {
    return visitReacton(reacton);
  } else if (schedule != nullptr) {
    return visitSchedule(schedule);
  } else if (print != nullptr) {
    return visitPrint(print);
  } else if (ifStmt != nullptr) {
    return visitIf(ifStmt);
  } else if (varDecl != nullptr) {
    return visitVariableDeclaration(varDecl);
  } else if (varAssign != nullptr) {
    return visitVariableAssignment(varAssign);
  }

  return Command();
}

std::any
ConcreteFiScriptVisitor::visitSchedule(FiScriptParser::ScheduleContext *ctx) {
  Command command;
  command.type = Command::CommandType::Schedule;
  command.arguments = std::any_cast<std::vector<std::string>>(
      visitArgumentList(ctx->argumentList()));

  for (const auto &statement : ctx->block()->statement()) {
    command.in_scope.push_back(
        std::any_cast<Command>(visitStatement(statement)));
  }
  return command;
}

std::any
ConcreteFiScriptVisitor::visitReacton(FiScriptParser::ReactonContext *ctx) {
  Command command;
  command.type = Command::CommandType::ReactOn;
  command.arguments = std::any_cast<std::vector<std::string>>(
      visitArgumentList(ctx->argumentList()));

  for (const auto &statement : ctx->block()->statement()) {
    command.in_scope.push_back(
        std::any_cast<Command>(visitStatement(statement)));
  }
  return command;
}

std::any
ConcreteFiScriptVisitor::visitPrint(FiScriptParser::PrintContext *ctx) {
  Command command;
  command.type = Command::CommandType::Print;

  auto expr_result = visitExpression(ctx->expression());
  command.expression = std::any_cast<std::shared_ptr<ExprNode>>(expr_result);

  return command;
}

std::any
ConcreteFiScriptVisitor::visitAlert(FiScriptParser::AlertContext *ctx) {
  Command command;
  command.type = Command::CommandType::Alert;

  auto expr_result = visitExpression(ctx->expression());
  command.expression = std::any_cast<std::shared_ptr<ExprNode>>(expr_result);
  return command;
}

std::any
ConcreteFiScriptVisitor::visitIf(FiScriptParser::IfContext *ctx) {
  Command command;
  command.type = Command::CommandType::If;

  auto expr_result = visitExpression(ctx->expression(0));
  command.expression = std::any_cast<std::shared_ptr<ExprNode>>(expr_result);

  for (const auto &statement : ctx->block(0)->statement()) {
    command.in_scope.push_back(
        std::any_cast<Command>(visitStatement(statement)));
  }

  size_t num_else_ifs = ctx->expression().size() - 1;
  for (size_t i = 0; i < num_else_ifs; ++i) {
    auto else_if_expr = visitExpression(ctx->expression(i + 1));
    auto else_if_expr_node = std::any_cast<std::shared_ptr<ExprNode>>(else_if_expr);

    std::vector<Command> else_if_block;
    for (const auto &statement : ctx->block(i + 1)->statement()) {
      else_if_block.push_back(std::any_cast<Command>(visitStatement(statement)));
    }

    command.else_if_branches.push_back({else_if_expr_node, else_if_block});
  }

  size_t total_blocks = ctx->block().size();
  size_t expected_blocks = 1 + num_else_ifs;
  if (total_blocks > expected_blocks) {
    for (const auto &statement : ctx->block(total_blocks - 1)->statement()) {
      command.else_block.push_back(std::any_cast<Command>(visitStatement(statement)));
    }
  }

  return command;
}

std::any ConcreteFiScriptVisitor::visitArgumentList(
    FiScriptParser::ArgumentListContext *ctx) {

  return visitArgumentComposition(ctx->argumentComposition());
}

std::any ConcreteFiScriptVisitor::visitArgumentComposition(
    FiScriptParser::ArgumentCompositionContext *ctx) {

  std::vector<std::string> output;
  const auto argument_list = ctx->argument();
  for (const auto &arg : argument_list) {
    output.push_back(arg->getText());
  }
  return output;
}

std::any ConcreteFiScriptVisitor::visitArgument(
    FiScriptParser::ArgumentContext *ctx) {
  return ctx->getText();
}

std::any ConcreteFiScriptVisitor::visitVariableDeclaration(
    FiScriptParser::VariableDeclarationContext *ctx) {
  Command command;
  command.type = Command::CommandType::VariableDeclaration;
  command.variable_name = ctx->IDENTIFIER()->getText();

  // the trick here is that it will call "visit(expression)" (check comments)
  // on ::visitExpression
  auto expr_result = visitExpression(ctx->expression());
  command.expression = std::any_cast<std::shared_ptr<ExprNode>>(expr_result);

  return command;
}

std::any ConcreteFiScriptVisitor::visitVariableAssignment(
    FiScriptParser::VariableAssignmentContext *ctx) {
  Command command;
  command.type = Command::CommandType::VariableAssignment;
  command.variable_name = ctx->IDENTIFIER()->getText();
  command.compound_op = ctx->COMPOUND_OP()->getText();

  auto expr_result = visitExpression(ctx->expression());
  command.expression = std::any_cast<std::shared_ptr<ExprNode>>(expr_result);

  return command;
}

std::any
ConcreteFiScriptVisitor::visitBlock(FiScriptParser::BlockContext *ctx) {
  std::vector<Command> output;
  for (const auto &statement : ctx->statement())
    output.push_back(std::any_cast<Command>(visitStatement(statement)));
  return output;
}

std::any ConcreteFiScriptVisitor::visitExpression(FiScriptParser::ExpressionContext *ctx) {
  // it will check what is the type of the expression DYNAMICALLY
  // thanks to alternative labels (see .g4 file with the #MulDiv etc...)
  // which will call visitXXX (where XXX is AlternativeLabel name)
  return visit(ctx);
}

std::any ConcreteFiScriptVisitor::visitParenExpression(FiScriptParser::ParenExpressionContext *ctx) {
  auto inner_result = visitExpression(ctx->expression());
  auto inner = std::any_cast<std::shared_ptr<ExprNode>>(inner_result);

  std::shared_ptr<ExprNode> node = std::make_shared<ParenExprNode>(inner);
  return node;
}

std::any ConcreteFiScriptVisitor::visitMulDiv(FiScriptParser::MulDivContext *ctx) {
  auto left_result = visitExpression(ctx->expression(0));
  auto right_result = visitExpression(ctx->expression(1));

  auto left = std::any_cast<std::shared_ptr<ExprNode>>(left_result);
  auto right = std::any_cast<std::shared_ptr<ExprNode>>(right_result);

  std::string op = ctx->children[1]->getText();

  std::shared_ptr<ExprNode> node = std::make_shared<BinaryOpNode>(op, left, right);
  return node;
}

std::any ConcreteFiScriptVisitor::visitAddSub(FiScriptParser::AddSubContext *ctx) {
  auto left_result = visitExpression(ctx->expression(0));
  auto right_result = visitExpression(ctx->expression(1));

  auto left = std::any_cast<std::shared_ptr<ExprNode>>(left_result);
  auto right = std::any_cast<std::shared_ptr<ExprNode>>(right_result);

  std::string op = ctx->children[1]->getText();

  std::shared_ptr<ExprNode> node = std::make_shared<BinaryOpNode>(op, left, right);
  return node;
}

std::any ConcreteFiScriptVisitor::visitVariableRef(FiScriptParser::VariableRefContext *ctx) {
  std::string var_name = ctx->IDENTIFIER()->getText();
  std::shared_ptr<ExprNode> node = std::make_shared<VariableRefNode>(var_name);
  return node;
}

std::any ConcreteFiScriptVisitor::visitNumericLiteral(FiScriptParser::NumericLiteralContext *ctx) {
  std::string value = ctx->NUMBER()->getText();
  std::shared_ptr<ExprNode> node = std::make_shared<LiteralNode>(value, false, false);
  return node;
}

std::any ConcreteFiScriptVisitor::visitStringLiteral(FiScriptParser::StringLiteralContext *ctx) {
  std::string value = ctx->STRING_LITERAL()->getText();
  std::shared_ptr<ExprNode> node = std::make_shared<LiteralNode>(value, true, false);
  return node;
}

std::any ConcreteFiScriptVisitor::visitBooleanLiteral(FiScriptParser::BooleanLiteralContext *ctx) {
  std::string value = ctx->BOOL_LITERAL()->getText();
  std::shared_ptr<ExprNode> node = std::make_shared<LiteralNode>(value, false, true);
  return node;
}

std::any ConcreteFiScriptVisitor::visitComparison(FiScriptParser::ComparisonContext *ctx) {
  auto left_result = visitExpression(ctx->expression(0));
  auto right_result = visitExpression(ctx->expression(1));

  auto left = std::any_cast<std::shared_ptr<ExprNode>>(left_result);
  auto right = std::any_cast<std::shared_ptr<ExprNode>>(right_result);

  std::string op = ctx->children[1]->getText();

  std::shared_ptr<ExprNode> node = std::make_shared<BinaryOpNode>(op, left, right);
  return node;
}

std::any ConcreteFiScriptVisitor::visitLogicalOp(FiScriptParser::LogicalOpContext *ctx) {
  auto left_result = visitExpression(ctx->expression(0));
  auto right_result = visitExpression(ctx->expression(1));

  auto left = std::any_cast<std::shared_ptr<ExprNode>>(left_result);
  auto right = std::any_cast<std::shared_ptr<ExprNode>>(right_result);

  std::string op = ctx->children[1]->getText();

  std::shared_ptr<ExprNode> node = std::make_shared<BinaryOpNode>(op, left, right);
  return node;
}
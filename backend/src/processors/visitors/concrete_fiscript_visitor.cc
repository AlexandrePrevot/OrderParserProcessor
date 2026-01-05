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
  FiScriptParser::VariableDeclarationContext *varDecl = ctx->variableDeclaration();
  FiScriptParser::VariableAssignmentContext *varAssign = ctx->variableAssignment();

  Command command;
  if (reacton != nullptr) {
    return visitReacton(reacton);
  } else if (schedule != nullptr) {
    return visitSchedule(schedule);
  } else if (print != nullptr) {
    command.type = Command::CommandType::Print;
    command.arguments =
        std::any_cast<std::vector<std::string>>(visitPrint(print));
  } else if (varDecl != nullptr) {
    return visitVariableDeclaration(varDecl);
  } else if (varAssign != nullptr) {
    return visitVariableAssignment(varAssign);
  }

  return command;
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
  return visitArgumentList(ctx->argumentList());
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

  // the trick here is that it will call "visit(expression)"
  // so it will check what is the type of the expression DYNAMICALLY
  // thanks to alternative labels (see .g4 file with the #MulDiv etc...)
  // which will call visitXXX (where XXX is AlternativeLabel name)
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
  std::shared_ptr<ExprNode> node = std::make_shared<LiteralNode>(value, false);
  return node;
}

std::any ConcreteFiScriptVisitor::visitStringLiteral(FiScriptParser::StringLiteralContext *ctx) {
  std::string value = ctx->STRING_LITERAL()->getText();
  std::shared_ptr<ExprNode> node = std::make_shared<LiteralNode>(value, true);
  return node;
}
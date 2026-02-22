#pragma once
#include "FiScriptVisitor.h"

#include <string>
#include <vector>
#include <map>

#include "command.h"

class ConcreteFiScriptVisitor : public FiScriptVisitor {
public:
  bool Compile(const std::string &code);
  std::vector<Command> get_commands_list() { return commands_list_; }

private:
  bool ScheduleCommand(const std::vector<std::string>& arguments);

  virtual std::any visitScript(FiScriptParser::ScriptContext *context);

  virtual std::any visitStatement(FiScriptParser::StatementContext *context);

  virtual std::any visitSchedule(FiScriptParser::ScheduleContext *context);

  virtual std::any visitReacton(FiScriptParser::ReactonContext *context);

  virtual std::any visitPrint(FiScriptParser::PrintContext *context);

  virtual std::any visitAlert(FiScriptParser::AlertContext *context);

  virtual std::any visitSendorder(FiScriptParser::SendorderContext *context);

  virtual std::any visitIf(FiScriptParser::IfContext *context);

  virtual std::any visitVariableDeclaration(FiScriptParser::VariableDeclarationContext *context);

  virtual std::any visitVariableAssignment(FiScriptParser::VariableAssignmentContext *context);

  virtual std::any
  visitArgumentList(FiScriptParser::ArgumentListContext *context);

  virtual std::any
  visitArgumentComposition(FiScriptParser::ArgumentCompositionContext *context);

  virtual std::any visitArgument(FiScriptParser::ArgumentContext *context);

  virtual std::any visitBlock(FiScriptParser::BlockContext *context);

  virtual std::any visitExpression(FiScriptParser::ExpressionContext *context);
  virtual std::any visitParenExpression(FiScriptParser::ParenExpressionContext *context);
  virtual std::any visitMulDiv(FiScriptParser::MulDivContext *context);
  virtual std::any visitAddSub(FiScriptParser::AddSubContext *context);
  virtual std::any visitComparison(FiScriptParser::ComparisonContext *context);
  virtual std::any visitLogicalOp(FiScriptParser::LogicalOpContext *context);
  virtual std::any visitVariableRef(FiScriptParser::VariableRefContext *context);
  virtual std::any visitNumericLiteral(FiScriptParser::NumericLiteralContext *context);
  virtual std::any visitStringLiteral(FiScriptParser::StringLiteralContext *context);
  virtual std::any visitBooleanLiteral(FiScriptParser::BooleanLiteralContext *context);

  std::vector<Command> commands_list_;
  std::map<std::string, VariableType> variable_types_;
};

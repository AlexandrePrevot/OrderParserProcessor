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

  commands_list_.push_back(std::any_cast<Command>(visitScript(tree)));

  return true;
}

std::any
ConcreteFiScriptVisitor::visitScript(FiScriptParser::ScriptContext *ctx) {
  FiScriptParser::ReactonContext *reacton = ctx->reacton();
  FiScriptParser::ScheduleContext *schedule = ctx->schedule();
  FiScriptParser::PrintContext *print = ctx->print();

  Command command;
  if (reacton != nullptr) {
    command.type = Command::CommandType::ReactOn;
    command.arguments =
        std::any_cast<std::vector<std::string>>(visitReacton(reacton));
  } else if (schedule != nullptr) {
    command.type = Command::CommandType::Schedule;
    command.arguments =
        std::any_cast<std::vector<std::string>>(visitSchedule(schedule));
  } else if (print != nullptr) {
    command.type = Command::CommandType::Print;
    command.arguments =
        std::any_cast<std::vector<std::string>>(visitPrint(print));
  }

  return command;
}

std::any
ConcreteFiScriptVisitor::visitSchedule(FiScriptParser::ScheduleContext *ctx) {
  return visitArgumentList(ctx->argumentList());
}

std::any
ConcreteFiScriptVisitor::visitReacton(FiScriptParser::ReactonContext *ctx) {
  return visitArgumentList(ctx->argumentList());
}

std::any ConcreteFiScriptVisitor::visitPrint(FiScriptParser::PrintContext *ctx) {
  return visitArgumentList(ctx->argumentList());
}

std::any ConcreteFiScriptVisitor::visitArgumentList(
    FiScriptParser::ArgumentListContext *ctx) {

  return visitArgumentComposition(ctx->argumentComposition());
}

std::any ConcreteFiScriptVisitor::visitArgumentComposition(
    FiScriptParser::ArgumentCompositionContext *ctx) {

  std::vector<std::string> output;
  const auto argument_list = ctx->ARGUMENT();
  for (const auto &arg : argument_list) {
    output.push_back(arg->getText());
  }
  return output;
}
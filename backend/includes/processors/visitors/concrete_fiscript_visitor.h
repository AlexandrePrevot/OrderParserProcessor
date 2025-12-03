#pragma once
#include "FiScriptVisitor.h"

#include <string>
#include <vector>

struct Command {
  enum CommandType { Schedule, ReactOn };

  CommandType type;
  std::vector<std::string> arguments;
};

class ConcreteFiScriptVisitor : public FiScriptVisitor {
public:
  bool Compile(const std::string &code);
  std::vector<Command> get_commands_list() { return commands_list_; }

private:
  bool ScheduleCommand(const std::vector<std::string>& arguments);

  virtual std::any visitScript(FiScriptParser::ScriptContext *context);

  virtual std::any visitSchedule(FiScriptParser::ScheduleContext *context);

  virtual std::any visitReacton(FiScriptParser::ReactonContext *context);

  virtual std::any
  visitArgumentList(FiScriptParser::ArgumentListContext *context);

  virtual std::any
  visitArgumentComposition(FiScriptParser::ArgumentCompositionContext *context);

  std::vector<Command> commands_list_;
};

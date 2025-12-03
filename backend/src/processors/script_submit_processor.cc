#include "processors/script_submit_processor.h"

#include <iostream>
#include <string>

#include "antlr/FiScriptLexer.h"
#include "antlr/FiScriptParser.h"
#include "processors/visitors/concrete_fiscript_visitor.h"

bool ScriptSubmitProcessor::Process(
    const internal::ScriptSubmitRequest &script_submit_request) {

  const bool parsed = Parse(script_submit_request.content());

  if (!parsed) {
    std::cout << "could not compile given code" << std::endl;
  } else {
    std::cout << "successfully compiled given code" << std::endl;
  }

  return parsed;
}

bool ScriptSubmitProcessor::Parse(const std::string &code) {
  ConcreteFiScriptVisitor visitor;
  const bool compiled = visitor.Compile(code);

  const auto &commands = visitor.get_commands_list();

  for (const auto &command : commands) {
    if (command.type == Command::CommandType::Schedule) {
      return ScheduleCommand(command.arguments);
    } else if (command.type == Command::CommandType::ReactOn) {
      std::cout << "Reacting on event" << std::endl;
    }
  }

  return compiled;
}

int InterpretInteger(const std::string &str) {
  if (str.empty())
    throw std::invalid_argument("no argument given in argument slot");

  for (int i = 0; i < str.size() - 1; i++) {
    if (str[i] < '0' || str[i] > '9')
      throw std::invalid_argument("should give a time in second");
  }

  if (str.back() != 's')
    throw std::invalid_argument(
        "should give a time in second (no s at the end)");

  // Convert to int
  int value = std::stoi(std::string(str));
  return value;
}

bool ScriptSubmitProcessor::ScheduleCommand(
    const std::vector<std::string> &args) {
  bool valid = true;
  if (args.size() != 2) {
    valid = false;
    std::cout
        << "expected 2 arguments, first when to start, second when to finish"
        << std::endl;
  }

  if (args.size() == 0)
    return false;

  int nbr = 0;
  try {
    nbr = InterpretInteger(args[1]);
  } catch (...) {
    std::cout << "could not interpret " << args[1]
              << " as an integer number of seconds (format should be XXXs)"
              << std::endl;
    valid = false;
  }

  if (!valid)
    return false;

  timer_manager_.CreateTimer(
      []() {
        std::cout << "TIMER DONE ! " << std::endl;
      }, // cannot give any action for now
      std::chrono::seconds(nbr), 1);
  return true;
}

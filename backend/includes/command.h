#pragma once

#include <vector>

struct Command {
  enum CommandType { Schedule, ReactOn, Print };

  CommandType type;
  std::vector<std::string> arguments;
  std::vector<Command> in_scope;
};
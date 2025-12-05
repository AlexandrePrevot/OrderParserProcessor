#include "processors/visitors/file_maker.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

FileMaker::FileMaker(const std::vector<Command> &commands) {
  code_.push_back({0, ""});
  code_.push_back({0, "int main() {"});
  code_.push_back({1, "return 0;"});
  code_.push_back({0, "}"});

  includes_it_ = code_.begin();
  code_it_ = code_.begin();
  code_it_++;
  code_it_++;
  tab_to_add_ = 0;

  for (const auto &command : commands) {
    if (!AddCommand(command))
      return;
  }
  BuildOutput();
}

void FileMaker::GenerateScript() const noexcept {
  if (output_.empty()) {
    std::cout << "nothing to generate" << std::endl;
    return;
  }

  // Call Python script (keep as you have it)
  std::stringstream cmd;
  cmd << "python3 $ORDER_PARSER_PROCESSOR_ROOT/backend/src/system_builder.py "
         "--includes \"test1\" --scriptName "
         "\"MyScriptAlexandrePrevot\"";

  std::string command = cmd.str();
  std::cout << "running command : " << command << std::endl;
  system(command.c_str());

  std::cout << "placing the main file" << std::endl;

  // Get environment variable
  const char *root = std::getenv("ORDER_PARSER_PROCESSOR_ROOT");
  if (!root) {
    std::cerr << "Environment variable ORDER_PARSER_PROCESSOR_ROOT not set!"
              << std::endl;
    return;
  }

  // Build full path
  std::filesystem::path mainPath =
      std::filesystem::path(root) / ".." / "output_bin" / "main.cc";

  // Ensure the directory exists
  std::filesystem::create_directories(mainPath.parent_path());

  // the main.cc file is placed by the cpp program
  // but the rest is done by the python program
  // everything should be handled by python
  // but it is very annoying to handle the "" of std::cout
  // when passing the code as arguments to the python file
  std::ofstream file(mainPath);
  if (!file) {
    std::cerr << "Failed to open file for writing: " << mainPath << std::endl;
    return;
  }
  file << output_;
}

bool FileMaker::AddCommand(const Command &command) { return MakeLine(command); }

bool FileMaker::MakeLine(const Command &command) {
  using Type = Command::CommandType;
  switch (command.type) {
  case Type::Schedule:
    return MakeScheduleCommand(command);
  case Type::Print:
    return MakePrintCommand(command);
  default:
    break;
  }
  return false;
}

void FileMaker::BuildOutput() {
  std::ostringstream oss;

  for (const auto &tab_code : code_) {
    const long tab = tab_code.first;
    const std::string &code = tab_code.second;

    for (int i = 0; i < tab; i++)
      oss << "\t";
    oss << code;
    oss << "\n";
  }

  output_ = std::move(oss.str());
}

// =========================== COMPILATION LOGIC ===========================

bool ValidInteger(const std::string &str) {
  if (str.empty()) {
    std::cout << "'" << str << "' is not an integer" << std::endl;
    return false;
  }

  for (int i = 0; i < str.size() - 1; i++) {
    if (str[i] < '0' || str[i] > '9') {
      std::cout << "'" << str << "' is not an integer" << std::endl;
      return false;
    }
  }

  if (str.front() == '0') {
    std::cout << "'" << str << "' is not an integer" << std::endl;
    return false;
  }

  return true;
}

int TimeInSecondToInteger(const std::string &str) {
  if (!ValidInteger(str.substr(0, str.size() - 1))) {
    throw std::invalid_argument("should give a time in second");
  }

  if (str.back() != 's')
    throw std::invalid_argument(
        "should give a time in second (no s at the end)");

  // Convert to int
  int value = std::stoi(std::string(str));
  return value;
}

const bool ScheduleArgsValid(const std::vector<std::string> &args) {
  bool valid = true;
  if (args.size() != 3) {
    std::cout << "expected 3 arguments, type of timer, second time to wait, "
                 "3rd repeatition"
              << std::endl;
    return false;
  }

  if (args.size() == 0)
    return false;

  int nbr = 0;
  try {
    nbr = TimeInSecondToInteger(args[1]);
  } catch (...) {
    std::cout << "could not interpret " << args[1]
              << " as an integer number of seconds (format should be XXXs)"
              << std::endl;
    valid = false;
  }

  if (!ValidInteger(args[2]))
    valid = false;

  return valid;
}

bool FileMaker::MakeScheduleCommand(const Command &command) {
  const std::vector<std::string> &args = command.arguments;

  if (!ScheduleArgsValid(args))
    return false;

  const int seconds_to_wait = TimeInSecondToInteger(args[1]);
  const int repeat = stoi(args[2]);

  long tab = code_it_->first;

  Include(command);

  const bool added_before = history_.find(command.type) != std::cend(history_);
  if (!added_before) {
    InsertCode("TimerManager timer_manager;", tab);
    InsertCode("timer_manager.WaitTillLast(0);", tab);
    code_it_--;
  }
  history_.insert(command.type);

  InsertCode("timer_manager.CreateTimer([]() {", tab);
  tab_to_add_++;
  for (const auto &sub_command : command.in_scope) {
    MakeLine(sub_command);
  }
  tab_to_add_--;
  InsertCode(std::string("}, ") + std::to_string(seconds_to_wait) + ", " +
                 std::to_string(repeat) + ");",
             tab);

  return true;
}

bool PrintArgsValid(const std::vector<std::string> &args) {
  if (args.size() != 1) {
    std::cout << "currently support only 1 argument for Print" << std::endl;
    return false;
  }

  if (args[0].size() < 2) {
    std::cout << "only strings are supported to print" << std::endl;
    return false;
  }

  if (args[0].front() != '"' || args[0].back() != '"') {
    std::cout << "only strings are supported to print (should start and end "
                 "with '\"')"
              << std::endl;
    return false;
  }

  return true;
}

bool FileMaker::MakePrintCommand(const Command &command) {
  if (!PrintArgsValid(command.arguments))
    return false;

  Include(command);
  history_.insert(Command::CommandType::Print);

  long tab = code_it_->first;
  InsertCode(std::string("std::cout << ") + command.arguments[0] +
                 std::string(" << std::endl;"),
             tab);

  return true;
}

void FileMaker::Include(const Command &command) {
  if (history_.find(command.type) != std::cend(history_))
    return;

  using Type = Command::CommandType;
  switch (command.type) {
  case Type::Schedule:
    InsertInclude("#include \"processors/common/timers.h\"");
    break;
  case Type::Print:
    InsertInclude("#include <iostream>");
  default:
    break;
  }
}

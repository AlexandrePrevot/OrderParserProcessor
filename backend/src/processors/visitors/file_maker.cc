#include "processors/visitors/file_maker.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

constexpr std::string_view kEndIncludes = "// ----- end includes";
constexpr std::string_view kBoolToStringTernary = " ? \"True\" : \"False\"";

FileMaker::FileMaker(const std::vector<Command> &commands) {
  compiled_ = true;
  code_.push_back({0, "#include <grpcpp/grpcpp.h>"});
  code_.push_back({0, "// ----- end includes"});
  code_.push_back({0, ""});
  SetGRPC();
  code_.push_back({0, ""});
  code_.push_back({0, "int main() {"});
  code_.push_back({1, "return 0;"});
  code_.push_back({0, "}"});

  includes_it_ = code_.begin();
  code_it_ = code_.begin();
  while (code_it_->second != "int main() {")
    code_it_++;
  code_it_++;
  tab_to_add_ = 0;

  for (const auto &command : commands) {
    if (!AddCommand(command)) {
      std::cout << "could not compile because of command " << command.type
                << std::endl;
      compiled_ = false;
      return;
    }
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
         "--scriptName \"MyScriptAlexandrePrevot\" --includes ";

  for (const std::string &include : includes_) {
    cmd << include;
    cmd << " ";
  }

  std::string command = cmd.str();
  std::cout << "running command : " << command << std::endl;
  system(command.c_str());

  std::cout << "placing the main file" << std::endl;

  // Get environment variable
  const char *root = std::getenv("ORDER_PARSER_PROCESSOR_ROOT");
  if (root == nullptr) {
    std::cerr << "Environment variable ORDER_PARSER_PROCESSOR_ROOT not set!"
              << std::endl;
    return;
  }

  std::filesystem::path env(root);

  // Build full path
  std::filesystem::path mainPath = env / ".." / "output_bin" / "main.cc";

  // Ensure the directory exists
  std::filesystem::create_directories(mainPath.parent_path());

  // the main.cc file is placed by the cpp program
  // but the rest is done by the python program
  // everything should be handled by python
  // but it is very annoying to handle the "" of std::cout
  // when passing the code as arguments to the python file
  {
    std::ofstream file(mainPath);
    if (!file) {
      std::cerr << "Failed to open file for writing: " << mainPath << std::endl;
      return;
    }
    file << output_;
    file.flush();
  }

  std::filesystem::path out = env / ".." / "output_bin";
  std::filesystem::path build = out / "build";

  std::filesystem::create_directories(build);

  // Change directory to build/
  std::filesystem::current_path(build);

  // CMake
  if (std::system("cmake ..") != 0) {
    std::cerr << "CMake failed\n";
    return;
  }

  // Make
  if (std::system("make -j 2") != 0) {
    std::cerr << "Make failed\n";
    return;
  }

  std::cout << "Build completed successfully!\n";
}

bool FileMaker::AddCommand(const Command &command) { return MakeLine(command); }

bool FileMaker::MakeLine(const Command &command) {
  using Type = Command::CommandType;
  switch (command.type) {
  case Type::Schedule:
    return MakeScheduleCommand(command);
  case Type::Print:
    return MakePrintCommand(command);
  case Type::Alert:
    return MakeAlertCommand(command);
  case Type::ReactOn:
    return MakeReactOnCommand(command);
  case Type::If:
    return MakeIfCommand(command);
  case Type::VariableDeclaration:
    return MakeVariableDeclaration(command);
  case Type::VariableAssignment:
    return MakeVariableAssignment(command);
  default:
    break;
  }
  return false;
}

void FileMaker::MakeBlock(const std::vector<Command> &commands) {
  auto prev_variable_types = variable_types_;
  tab_to_add_++;
  for (const auto &sub_command : commands) {
    MakeLine(sub_command);
  }
  tab_to_add_--;
  variable_types_ = std::move(prev_variable_types);
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

  if (args.size() == 0) {
    std::cout << "no arguments given to create timer" << std::endl;
    return false;
  }

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

  std::cout << "adding the schedule command" << std::endl;

  const int seconds_to_wait = TimeInSecondToInteger(args[1]);
  const int repeat = stoi(args[2]);

  long tab = code_it_->first;

  AddTimerManager(command);

  CollectRequiredManagers(command);

  std::string lambda_captures = GenerateLambdaCaptures();

  InsertCode("timer_manager.CreateTimer(" + lambda_captures + "() mutable {", tab);

  MakeBlock(command.in_scope);

  InsertCode(std::string("}, std::chrono::seconds(") +
                 std::to_string(seconds_to_wait) + "), " +
                 std::to_string(repeat) + ");",
             tab);

  return true;
}

bool FileMaker::MakePrintCommand(const Command &command) {
  if (!command.expression) {
    std::cout << "Print command missing expression" << std::endl;
    return false;
  }

  std::cout << "adding the print command" << std::endl;

  Include(command);

  long tab = code_it_->first;

  std::string expr_code = GenerateExpressionCode(command.expression.get(), VariableType::String);
  InsertCode(std::string("std::cout << (") + expr_code + std::string(") << std::endl;"),
             tab);

  return true;
}

bool FileMaker::MakeAlertCommand(const Command &command) {
  if (!command.expression) {
    std::cout << "Alert command missing expression" << std::endl;
    return false;
  }

  std::cout << "adding the alert command" << std::endl;

  long tab = code_it_->first;
  
  AddAlertService(command);


  std::string expr_code = GenerateExpressionCode(command.expression.get(), VariableType::String);
  InsertCode(std::string("script_alert_service.SendAlert(\"User\", ") + expr_code + ");",
             tab);

  return true;
}

bool ReactOnArgsValid(const std::vector<std::string> &args) {
  if (args.size() != 2) {
    std::cout << "expected 2 arguments: instrument_id and repetition count"
              << std::endl;
    return false;
  }

  // First argument should be a string (instrument_id)
  if (args[0].size() < 2 || args[0].front() != '"' || args[0].back() != '"') {
    std::cout << "first argument (instrument_id) should be a string"
              << std::endl;
    return false;
  }

  // Second argument should be an integer or -1
  const std::string &repeat_str = args[1];
  if (repeat_str == "-1") {
    return true;  // -1 is valid for infinite
  }

  if (!ValidInteger(repeat_str)) {
    std::cout << "second argument (repetition count) should be an integer or -1"
              << std::endl;
    return false;
  }

  return true;
}

bool FileMaker::MakeReactOnCommand(const Command &command) {
  const std::vector<std::string> &args = command.arguments;

  if (!ReactOnArgsValid(args))
    return false;

  std::cout << "adding the reacton command" << std::endl;

  const std::string &instrument_id = args[0];
  const int repeat = std::stoi(args[1]);

  long tab = code_it_->first;

  AddReactOnService(command);

  CollectRequiredManagers(command);

  std::string lambda_captures = GenerateLambdaCaptures();

  InsertCode(std::string("reacton_service.RegisterReaction(") + instrument_id +
                 ", " + std::to_string(repeat) + ", " + lambda_captures +
                 "(const internal::PriceUpdate &quote) mutable {",
             tab);

  MakeBlock(command.in_scope);

  InsertCode("});", tab);

  return true;
}

bool FileMaker::MakeIfCommand(const Command &command) {
  if (!command.expression) {
    std::cout << "If command missing expression" << std::endl;
    return false;
  }

  std::cout << "adding the if command" << std::endl;

  long tab = code_it_->first;

  CollectRequiredManagers(command);

  std::string condition = GenerateExpressionCode(command.expression.get(), VariableType::Boolean);
  InsertCode("if (" + condition + ") {", tab);

  MakeBlock(command.in_scope);

  for (const auto &else_if_branch : command.else_if_branches) {
    std::string else_if_condition = GenerateExpressionCode(else_if_branch.first.get(), VariableType::Boolean);
    InsertCode("} else if (" + else_if_condition + ") {", tab);

    MakeBlock(else_if_branch.second);
  }

  if (!command.else_block.empty()) {
    InsertCode("} else {", tab);

    MakeBlock(command.else_block);
  }

  InsertCode("}", tab);

  return true;
}

VariableType FileMaker::InferExpressionType(const ExprNode* expr) const {
  if (!expr) {
    return VariableType::Numeric;
  }

  switch (expr->node_type) {
    case ExprNode::Type::Literal: {
      const auto* lit = static_cast<const LiteralNode*>(expr);
      if (lit->is_boolean) {
        return VariableType::Boolean;
      }
      return lit->is_string ? VariableType::String : VariableType::Numeric;
    }

    case ExprNode::Type::VariableRef: {
      const auto* var_ref = static_cast<const VariableRefNode*>(expr);
      auto it = variable_types_.find(var_ref->var_name);
      if (it != variable_types_.end()) {
        return it->second;
      }
      return VariableType::Numeric;
    }

    case ExprNode::Type::BinaryOp: {
      const auto* binop = static_cast<const BinaryOpNode*>(expr);
      VariableType left_type = InferExpressionType(binop->left.get());
      VariableType right_type = InferExpressionType(binop->right.get());

      if (binop->op == ">" || binop->op == "<" || binop->op == ">=" ||
          binop->op == "<=" || binop->op == "==" || binop->op == "!=") {
        return VariableType::Boolean;
      }

      if (binop->op == "and" || binop->op == "or") {
        return VariableType::Boolean;
      }

      if (binop->op == "+" || binop->op == "-") {
        if (left_type == VariableType::String || right_type == VariableType::String) {
          return VariableType::String;
        }
      }
      return left_type;
    }

    case ExprNode::Type::Paren: {
      const auto* paren = static_cast<const ParenExprNode*>(expr);
      return InferExpressionType(paren->inner.get());
    }

    default:
      return VariableType::Numeric;
  }
}

std::string FileMaker::GenerateExpressionCode(const ExprNode* expr, VariableType context_type) const {
  if (!expr) {
    return "";
  }

  switch (expr->node_type) {
    case ExprNode::Type::Literal: {
      const auto* lit = static_cast<const LiteralNode*>(expr);

      if (lit->is_boolean) {
        std::string cpp_bool = (lit->value == "True") ? "true" : "false";
        if (context_type == VariableType::String) {
          return "std::string(\"" + lit->value + "\")";
        }
        return cpp_bool;
      }

      if (context_type == VariableType::String) {
        if (lit->is_string) {
          return "std::string(" + lit->value + ")";
        } else {
          return "std::to_string(" + lit->value + ")";
        }
      } else {
        return lit->value;
      }
    }

    case ExprNode::Type::VariableRef: {
      const auto* var_ref = static_cast<const VariableRefNode*>(expr);
      auto it = variable_types_.find(var_ref->var_name);
      if (context_type == VariableType::String) {
        if (it != variable_types_.end()) {
          if (it->second == VariableType::Numeric) {
            return "std::to_string(" + var_ref->var_name + ")";
          } else if (it->second == VariableType::Boolean) {
            return "(" + var_ref->var_name + std::string(kBoolToStringTernary) + ")";
          }
        } else { // currently only in the case of class attribute such as quote.price
          return "std::to_string(" + var_ref->var_name + "())";
        }
      }

      // if not declared, then we are working with
      // a class attribute such as quote.price
      if (it == variable_types_.end()) {
        return var_ref->var_name + "()";
      }

      return var_ref->var_name;
    }

    case ExprNode::Type::BinaryOp: {
      const auto* binop = static_cast<const BinaryOpNode*>(expr);
      VariableType left_type = InferExpressionType(binop->left.get());
      VariableType right_type = InferExpressionType(binop->right.get());

      if (binop->op == "and") {
        std::string left_code = GenerateExpressionCode(binop->left.get(), VariableType::Boolean);
        std::string right_code = GenerateExpressionCode(binop->right.get(), VariableType::Boolean);
        std::string result = left_code + " && " + right_code;
        if (context_type == VariableType::String) {
          return "(" + result + std::string(kBoolToStringTernary) + ")";
        }
        return result;
      } else if (binop->op == "or") {
        std::string left_code = GenerateExpressionCode(binop->left.get(), VariableType::Boolean);
        std::string right_code = GenerateExpressionCode(binop->right.get(), VariableType::Boolean);
        std::string result = left_code + " || " + right_code;
        if (context_type == VariableType::String) {
          return "(" + result + std::string(kBoolToStringTernary) + ")";
        }
        return result;
      } else if (binop->op == ">" || binop->op == "<" || binop->op == ">=" ||
                 binop->op == "<=" || binop->op == "==" || binop->op == "!=") {
        std::string left_code = GenerateExpressionCode(binop->left.get(), left_type);
        std::string right_code = GenerateExpressionCode(binop->right.get(), right_type);
        std::string result = left_code + " " + binop->op + " " + right_code;
        if (context_type == VariableType::String) {
          return "(" + result + std::string(kBoolToStringTernary) + ")";
        }
        return result;
      } else if (binop->op == "+" && (left_type == VariableType::String || right_type == VariableType::String)) {
        std::string left_code = GenerateExpressionCode(binop->left.get(), VariableType::String);
        std::string right_code = GenerateExpressionCode(binop->right.get(), VariableType::String);
        return left_code + " + " + right_code;
      } else if (binop->op == "*" || binop->op == "/") {
        std::string left_code = GenerateExpressionCode(binop->left.get(), VariableType::Numeric);
        std::string right_code = GenerateExpressionCode(binop->right.get(), VariableType::Numeric);
        return left_code + " " + binop->op + " " + right_code;
      } else {
        std::string left_code = GenerateExpressionCode(binop->left.get(), context_type);
        std::string right_code = GenerateExpressionCode(binop->right.get(), context_type);
        return left_code + " " + binop->op + " " + right_code;
      }
    }

    case ExprNode::Type::Paren: {
      const auto* paren = static_cast<const ParenExprNode*>(expr);
      VariableType paren_result_type = InferExpressionType(paren->inner.get());
      std::string inner_code = GenerateExpressionCode(paren->inner.get(), paren_result_type);
      std::string paren_expr = "(" + inner_code + ")";

      if (context_type == VariableType::String && paren_result_type == VariableType::Numeric) {
        return "std::to_string" + paren_expr;
      } else if (context_type == VariableType::String && paren_result_type == VariableType::String) {
        return "std::string" + paren_expr;
      } else if (context_type == VariableType::String && paren_result_type == VariableType::Boolean) {
        return "(" + paren_expr + std::string(kBoolToStringTernary) + ")";
      }
      return paren_expr;
    }

    default:
      return "";
  }
}

bool FileMaker::MakeVariableDeclaration(const Command &command) {
  long tab = code_it_->first;

  VariableType var_type = InferExpressionType(command.expression.get());

  auto it = variable_types_.find(command.variable_name);
  bool already_declared = it != variable_types_.end();

  variable_types_[command.variable_name] = var_type;

  if (var_type == VariableType::String) {
    std::string expr_code = GenerateExpressionCode(command.expression.get(), VariableType::String);
    if (already_declared) {
      InsertCode(command.variable_name + " = " + expr_code + ";", tab);
    } else {
      InsertCode("std::string " + command.variable_name + " = " + expr_code + ";", tab);
      const bool string_included_before = history_.find(Command::CommandType::VariableDeclaration) != std::cend(history_);
      if (!string_included_before) {
        InsertInclude("<string>", false);
        history_.insert(Command::CommandType::VariableDeclaration);
      }
    }
  } else if (var_type == VariableType::Boolean) {
    std::string expr_code = GenerateExpressionCode(command.expression.get(), VariableType::Boolean);
    if (already_declared) {
      InsertCode(command.variable_name + " = " + expr_code + ";", tab);
    } else {
      InsertCode("bool " + command.variable_name + " = " + expr_code + ";", tab);
    }
  } else {
    std::string expr_code = GenerateExpressionCode(command.expression.get(), VariableType::Numeric);
    if (already_declared) {
      InsertCode(command.variable_name + " = " + expr_code + ";", tab);
    } else {
      InsertCode("double " + command.variable_name + " = " + expr_code + ";", tab);
    }
  }

  return true;
}

bool FileMaker::MakeVariableAssignment(const Command &command) {
  long tab = code_it_->first;

  auto it = variable_types_.find(command.variable_name);
  if (it == variable_types_.end()) {
    return false;
  }

  VariableType var_type = it->second;

  if (var_type == VariableType::String) {
    std::string expr_code = GenerateExpressionCode(command.expression.get(), VariableType::String);
    if (command.compound_op == "+=") {
      InsertCode(command.variable_name + " += " + expr_code + ";", tab);
    } else {
      InsertCode(command.variable_name + " = " + expr_code + ";", tab);
    }
  } else if (var_type == VariableType::Boolean) {
    std::string expr_code = GenerateExpressionCode(command.expression.get(), VariableType::Boolean);
    InsertCode(command.variable_name + " = " + expr_code + ";", tab);
  } else {
    std::string expr_code = GenerateExpressionCode(command.expression.get(), VariableType::Numeric);
    InsertCode(command.variable_name + " " + command.compound_op + " " + expr_code + ";", tab);
  }

  return true;
}

void FileMaker::AddAlertService(const Command &command) {
  const bool added_before =
      history_.find(Command::CommandType::Alert) != std::cend(history_);
  const long tab = code_it_->first;
  Include(command);

  if (!added_before) {
    InsertCode("ScriptAlertService script_alert_service;", tab);
  }
}

void FileMaker::AddTimerManager(const Command &command) {
  const bool added_before =
      history_.find(Command::CommandType::Schedule) != std::cend(history_);
  const long tab = code_it_->first;
  Include(command);

  if (!added_before) {
    InsertCode("TimerManager timer_manager;", tab);
    InsertCode("timer_manager.WaitTillLast(0);", tab);
    code_it_--;
  }
}

void FileMaker::AddReactOnService(const Command &command) {
  const bool added_before =
      history_.find(Command::CommandType::ReactOn) != std::cend(history_);
  const long tab = code_it_->first;
  Include(command);

  if (!added_before) {
    InsertCode("ReactOnService reacton_service;", tab);
    InsertCode("reacton_service.WaitForCompletion();", tab);
    code_it_--;
  }
}

void FileMaker::Include(const Command &command) {
  if (history_.find(command.type) != std::cend(history_))
    return;

  using Type = Command::CommandType;
  switch (command.type) {
  case Type::Schedule:
    InsertInclude("\"processors/common/timers.h\"", true);
    break;
  case Type::Print:
    InsertInclude("<iostream>", false);
    break;
  case Type::Alert:
    InsertInclude("\"services/script_alert_service.h\"", true);
    break;
  case Type::ReactOn:
    InsertInclude("\"services/reacton_service.h\"", true);
    break;
  default:
    break;
  }

  history_.insert(command.type);
}

void FileMaker::ProcessCommandsForManagers(const std::vector<Command> &commands) {
  using Type = Command::CommandType;

  for (const auto &sub_command : commands) {
    if (sub_command.type == Type::Schedule) {
      active_managers_.insert(Type::Schedule);
      AddTimerManager(sub_command);
    } else if (sub_command.type == Type::ReactOn) {
      active_managers_.insert(Type::ReactOn);
      AddReactOnService(sub_command);
    } else if (sub_command.type == Type::Alert) {
      active_managers_.insert(Type::Alert);
      AddAlertService(sub_command);
    }
    CollectRequiredManagers(sub_command);
  }
}

// to avoid defining managers to soon (which take multiple threads)
// it is better to recursively check the subcommand
// to know if they are using the manager/service (such as Timer or ReactOn)
// then create them in the code
void FileMaker::CollectRequiredManagers(const Command &command) {
  ProcessCommandsForManagers(command.in_scope);

  for (const auto &else_if_branch : command.else_if_branches) {
    ProcessCommandsForManagers(else_if_branch.second);
  }

  ProcessCommandsForManagers(command.else_block);
}

std::string FileMaker::GenerateLambdaCaptures() const {
  std::string captures = "[=";

  if (active_managers_.find(Command::CommandType::Schedule) != active_managers_.end() &&
      history_.find(Command::CommandType::Schedule) != history_.end()) {
    captures += ", &timer_manager";
  }

  if (active_managers_.find(Command::CommandType::ReactOn) != active_managers_.end() &&
      history_.find(Command::CommandType::ReactOn) != history_.end()) {
    captures += ", &reacton_service";
  }

  if (active_managers_.find(Command::CommandType::Alert) != active_managers_.end() &&
      history_.find(Command::CommandType::Alert) != history_.end()) {
    captures += ", &script_alert_service";
  }

  captures += "]";
  return captures;
}

void FileMaker::SetGRPC() {
  code_.push_back({0, "void RunServer() {"});
  code_.push_back({1, "const std::string server_address(\"0.0.0.0:50051\");"});
  code_.push_back({1, "grpc::ServerBuilder builder;"});
  code_.push_back({1, "builder.AddListeningPort(server_address, "
                      "grpc::InsecureServerCredentials());"});
  code_.push_back({0, ""});
  code_.push_back(
      {1, "std::unique_ptr<grpc::Server> server(builder.BuildAndStart());"});
  code_.push_back({1, "server->Wait();"});
  code_.push_back({0, "}"});
}
#pragma once

#include <list>
#include <map>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "command.h"

class FileMaker {
public:
  FileMaker(const std::vector<Command> &commands);

  inline std::string GetCode() const { return output_; }
  inline bool Compiled() const { return compiled_; }

  void GenerateScript() const noexcept;

private:
  bool AddCommand(const Command &command);
  bool MakeLine(const Command &command);
  void BuildOutput();

  bool MakeScheduleCommand(const Command &command);
  bool MakePrintCommand(const Command &command);
  bool MakeReactOnCommand(const Command &command);
  bool MakeVariableDeclaration(const Command &command);
  bool MakeVariableAssignment(const Command &command);

  void AddTimerManager(const Command &command);
  void AddReactOnService(const Command &command);

  VariableType InferExpressionType(const ExprNode* expr) const;
  std::string GenerateExpressionCode(const ExprNode* expr, VariableType context_type) const;

  void Include(const Command &command);
  void CollectRequiredManagers(const Command &command);
  std::string GenerateLambdaCaptures() const;

  inline void InsertCode(const std::string &code, long tab) {
    code_.insert(code_it_, {tab + tab_to_add_, code});
  }

  inline void InsertInclude(const std::string &name, bool save) {
    code_.insert(includes_it_, {0, "#include " + name});
    if (save) {
      includes_.insert(name);
    }
  }

  void SetGRPC();

  std::list<std::pair<long, std::string>> code_;
  std::unordered_set<std::string> includes_;
  std::unordered_set<Command::CommandType> history_;
  std::unordered_set<Command::CommandType> active_managers_;
  std::map<std::string, VariableType> variable_types_;
  std::string output_;
  bool compiled_;
  long tab_to_add_;

  // to know at which line the includes are
  std::list<std::pair<long, std::string>>::iterator includes_it_;
  // to know at which line the code in main is
  std::list<std::pair<long, std::string>>::iterator code_it_;
};
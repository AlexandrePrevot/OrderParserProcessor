#pragma once

#include <list>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "command.h"

class FileMaker {
public:
  FileMaker(const std::vector<Command> &commands);

  inline std::string GetCode() const { return output_; }

private:
  bool AddCommand(const Command &command);
  bool MakeLine(const Command &command);
  void BuildOutput();

  bool MakeScheduleCommand(const Command &command);
  bool MakePrintCommand(const Command &command);

  void Include(const Command &command);
  void PlaceInclude(const std::string &include);

  inline void InsertCode(const std::string &code, long tab) {
    code_.insert(code_it_, {tab + tab_to_add_, code});
  }

  inline void InsertInclude(const std::string &include) {
    code_.insert(includes_it_, {0, include});
  }

  std::list<std::pair<long, std::string>> code_;
  std::unordered_set<Command::CommandType> need_include_;
  std::unordered_set<Command::CommandType> history_;
  std::string output_;
  long tab_to_add_;

  // to know at which line the includes are
  std::list<std::pair<long, std::string>>::iterator includes_it_;
  // to know at which line the code in main is
  std::list<std::pair<long, std::string>>::iterator code_it_;
};

/*
class FileMaker {
    // note : don't work with keyword, but with enums
    - map(keyword that needs include)
    - map(includes)
    - list(string) // will represents the lines

    addCommand(Command) {

        list[idx_commands] = MakeLine(command)
    }

    MakeLine(command) {
        (logic)

        if (NeedsInclude(keyword))
            map(includes).add(keyword)
    }

    NeedsInclude(string) {
        if (map(include).contain(string)) return false
        if (map(keyword that needs include).contains(string)) return true;

        return false;
    }

    // adding at idx means pushing the rest 1 idx further
    long idx_includes
    long idx_commands
}




Schedule(Start, 3s, 3) {
    Print("heyyyy")
}


->


// generate this main.cc
#include "common/timers.h"
#include <chrono>
#include <iostream> // make a map of included keywords

int main() {
    TimerManager m;

    m.CreateTimer([](){ std::cout << "heyyyy" << std::endl; }, seconds(3), 3);
    m.WaitTillLast(0);
    return 0;
}





*/
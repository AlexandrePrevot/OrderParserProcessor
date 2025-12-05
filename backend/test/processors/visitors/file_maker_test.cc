#include <gtest/gtest.h>
#include <iostream>

#include "processors/visitors/file_maker.h"

TEST(FileMakerTest, MakeCppFile) {
  Command c1;
  c1.type = Command::CommandType::Schedule;
  c1.arguments = {"Start", "3s", "3"};

  Command c2;
  c2.type = Command::CommandType::Schedule;
  c2.arguments = {"Start", "18s", "1"};

  Command c3;
  c3.type = Command::CommandType::Print;
  c3.arguments = {"\"a print used as an example !\""};

  c2.in_scope.push_back(c3);

  c1.in_scope.push_back(c2);

  FileMaker fm({c1, c2});

  std::cout << fm.GetCode() << std::endl;
  fm.GenerateScript();
}
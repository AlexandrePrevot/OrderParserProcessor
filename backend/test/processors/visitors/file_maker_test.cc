#include <gtest/gtest.h>
#include <iostream>

#include "processors/visitors/file_maker.h"

TEST(FileMakerTest, MakeCppFile) {
  Command c1;
  c1.type = Command::CommandType::Schedule;
  c1.arguments = {"Start", "1s", "3"};

  Command c2;
  c2.type = Command::CommandType::Print;
  c2.arguments = {"\"SIX SEVEN\""};

  Command c3;
  c3.type = Command::CommandType::Print;
  c3.arguments = {"\"This is a good example of the testing\""};

  c1.in_scope.push_back(c2);

  FileMaker fm({c1, c2, c3});

  std::cout << fm.GetCode() << std::endl;
  fm.GenerateScript();
}
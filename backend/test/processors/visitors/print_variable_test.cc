#include <gtest/gtest.h>
#include "processors/visitors/concrete_fiscript_visitor.h"
#include "processors/visitors/file_maker.h"

TEST(PrintVariableTest, SimpleStringVariable) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "myStr = \"test_str\"\n"
    "Print(myStr + \" that is being tested\")\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());

  FileMaker maker(commands, "test_user", "test_script");
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code:\n" << generated << std::endl;

  EXPECT_NE(generated.find("std::string myStr"), std::string::npos);
  EXPECT_NE(generated.find("std::cout << (myStr + std::string(\" that is being tested\"))"), 
            std::string::npos);
}

TEST(PrintVariableTest, NumericExpression) {
  ConcreteFiScriptVisitor visitor;

  std::string code =
    "x = 5 + 3\n"
    "Print(\"Result: \" + x)\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(2, commands.size());

  FileMaker maker(commands, "test_user", "test_script");
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code:\n" << generated << std::endl;

  EXPECT_NE(generated.find("double x"), std::string::npos);
  EXPECT_NE(generated.find("std::cout << (std::string(\"Result: \") + std::to_string(x))"), 
            std::string::npos);
}

TEST(PrintVariableTest, PlainStringLiteral) {
  ConcreteFiScriptVisitor visitor;

  std::string code = "Print(\"Hello World\")\n";

  EXPECT_TRUE(visitor.Compile(code));

  const auto &commands = visitor.get_commands_list();
  ASSERT_EQ(1, commands.size());

  FileMaker maker(commands, "test_user", "test_script");
  EXPECT_TRUE(maker.Compiled());

  std::string generated = maker.GetCode();
  std::cout << "Generated code:\n" << generated << std::endl;

  EXPECT_NE(generated.find("std::cout << (std::string(\"Hello World\"))"), 
            std::string::npos);
}

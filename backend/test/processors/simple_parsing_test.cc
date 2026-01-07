#include <gtest/gtest.h>
#include <string_view>

#include "processors/script_submit_processor.h"

// -----------------------------------------------------------------------------
// Test constants
// -----------------------------------------------------------------------------
namespace {

constexpr std::string_view kScheduleCorrect = "Schedule(Start, 3s, 3) {"
                                              "Print(\"hello world !\")"
                                              "}";
constexpr std::string_view kScheduleWrongArgType =
    "Schedule(Start, 23As1s, as) {}";
constexpr std::string_view kScheduleWrongArgNumber =
    "Schedule(Start, 3s, another, another) {}";
constexpr std::string_view kScheduleNotProperlyWritten =
    "Shcedule(Start, 3s, 3) {}";
constexpr std::string_view kCorrectPrint = "Print(\"hello world 123\")";
constexpr std::string_view kIncorrectPrint = "Print(hello world\")";
constexpr std::string_view kPrintWithNumericLiteral = "Print(356)";
constexpr std::string_view kPrintWithVariable = "x = 5\nPrint(\"Result: \" + x)";
constexpr std::string_view kPrintWithExpression = "Print(\"hello\" + \" \" + \"world\")";
constexpr std::string_view kPrintNotProperlyWritten =
    "Prin(\"hello world! 123\")";
constexpr std::string_view kWithQuotes = "Prin(\"hello \"world\"! 123\")";

struct ScriptTestCase {
  const char *name;
  std::string_view script;
  bool expected_success;
};

} // namespace

// -----------------------------------------------------------------------------
// Table-driven test
// -----------------------------------------------------------------------------
TEST(ScriptSubmit, TableDrivenValidation) {
  using namespace std;

  std::vector<ScriptTestCase> cases = {
      {"CorrectPrint", kCorrectPrint, true},
      {"IncorrectPrint", kIncorrectPrint, false},
      {"PrintWithNumericLiteral", kPrintWithNumericLiteral, true},
      {"PrintWithVariable", kPrintWithVariable, true},
      {"PrintWithExpression", kPrintWithExpression, true},
      {"PrintNotProperlyWritten", kPrintNotProperlyWritten, false},
      {"kWithQuotes", kWithQuotes, false},
      {"CorrectSchedule", kScheduleCorrect, true},
      {"ScheduleWrongArgType", kScheduleWrongArgType, false},
      {"ScheduleWrongArgNumber", kScheduleWrongArgNumber, false},
      {"MisspelledScheduleKeyword", kScheduleNotProperlyWritten, false},
  };

  for (const auto &tc : cases) {
    SCOPED_TRACE(tc.name);

    internal::ScriptSubmitRequest req;
    req.set_content(std::string{tc.script});

    ScriptSubmitProcessor processor;
    EXPECT_EQ(processor.Process(req), tc.expected_success);
  }
}

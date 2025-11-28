#include <gtest/gtest.h>
#include <string_view>

#include "processors/script_submit_processor.h"

// -----------------------------------------------------------------------------
// Test constants
// -----------------------------------------------------------------------------
namespace {

constexpr std::string_view kScheduleCorrect            = "Schedule(Start, 3s)";
constexpr std::string_view kScheduleWrongArgType       = "Schedule(Start, 23As1s)";
constexpr std::string_view kScheduleWrongArgNumber     = "Schedule(Start, 3s, another)";
constexpr std::string_view kScheduleNotProperlyWritten = "Shcedule(Start, 3s)";

struct ScriptTestCase {
    const char* name;
    std::string_view script;
    bool expected_success;
};

}  // namespace

// -----------------------------------------------------------------------------
// Table-driven test
// -----------------------------------------------------------------------------
TEST(ScriptSubmit, TableDrivenValidation) {
    using namespace std;

    std::vector<ScriptTestCase> cases = {
        {"CorrectSchedule",            kScheduleCorrect,            true },
        {"WrongArgType",               kScheduleWrongArgType,       false},
        {"WrongArgNumber",             kScheduleWrongArgNumber,     false},
        {"MisspelledScheduleKeyword",  kScheduleNotProperlyWritten, false},
    };

    for (const auto& tc : cases) {
        SCOPED_TRACE(tc.name);

        internal::ScriptSubmitRequest req;
        req.set_content(std::string{tc.script});

        ScriptSubmitProcessor processor;
        EXPECT_EQ(processor.Process(req), tc.expected_success);
    }
}

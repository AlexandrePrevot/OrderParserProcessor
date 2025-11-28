#include <gtest/gtest.h>


#include "processors/script_submit_processor.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    
  ScriptSubmitProcessor processor;
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
#include <gtest/gtest.h>
#include <cstring>
#include "market_data_point.h"

TEST(MarketDataPointTest, IsTriviallyCopyable) {
  EXPECT_TRUE(std::is_trivially_copyable<MarketDataPoint>::value);
}

TEST(MarketDataPointTest, DefaultConstructor) {
  MarketDataPoint point;

  EXPECT_EQ(point.price, 0.0);
  EXPECT_EQ(point.quantity, 0);
  EXPECT_EQ(point.timestamp_seconds, 0);
  EXPECT_EQ(point.timestamp_nanos, 0);
  EXPECT_EQ(point.instrument_id[0], '\0');
}

TEST(MarketDataPointTest, SetInstrumentId) {
  MarketDataPoint point;
  const char* test_id = "AAPL";

  point.set_instrument_id(test_id);

  EXPECT_STREQ(point.instrument_id, test_id);
}

TEST(MarketDataPointTest, InstrumentIdTruncation) {
  MarketDataPoint point;
  const char* long_id = "THIS_IS_A_VERY_LONG_INSTRUMENT_ID_OVER_32_CHARACTERS_LONG";

  point.set_instrument_id(long_id);

  EXPECT_EQ(std::strlen(point.instrument_id), 31);
  EXPECT_EQ(point.instrument_id[31], '\0');
}

TEST(MarketDataPointTest, InstrumentIdNullTermination) {
  MarketDataPoint point;
  const char* exact_31_chars = "1234567890123456789012345678901";

  point.set_instrument_id(exact_31_chars);

  EXPECT_STREQ(point.instrument_id, exact_31_chars);
  EXPECT_EQ(point.instrument_id[31], '\0');
}

TEST(MarketDataPointTest, SetAllFields) {
  MarketDataPoint point;

  point.price = 150.25;
  point.quantity = 100;
  point.timestamp_seconds = 1234567890;
  point.timestamp_nanos = 123456789;
  point.set_instrument_id("GOOGL");

  EXPECT_EQ(point.price, 150.25);
  EXPECT_EQ(point.quantity, 100);
  EXPECT_EQ(point.timestamp_seconds, 1234567890);
  EXPECT_EQ(point.timestamp_nanos, 123456789);
  EXPECT_STREQ(point.instrument_id, "GOOGL");
}

TEST(MarketDataPointTest, CopyConstructor) {
  MarketDataPoint point1;
  point1.price = 200.50;
  point1.quantity = 50;
  point1.set_instrument_id("MSFT");

  MarketDataPoint point2 = point1;

  EXPECT_EQ(point2.price, 200.50);
  EXPECT_EQ(point2.quantity, 50);
  EXPECT_STREQ(point2.instrument_id, "MSFT");
}

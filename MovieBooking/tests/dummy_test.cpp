#include <gtest/gtest.h>

#include "mbooking/movie_booking.h"


TEST(HelloTest, BasicAssertions)
{
	bool b = foo(-4);

	EXPECT_STRNE("hello", "world");
	EXPECT_EQ(7 * 6, 42);
}

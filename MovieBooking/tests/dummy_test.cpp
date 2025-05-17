#include <gtest/gtest.h>

#include "mbooking/movie_booking.h"


TEST(HelloTest, BasicAssertions)
{
	foo();

	EXPECT_STRNE("hello", "world");
	EXPECT_EQ(7 * 6, 42);
}

#include <aegis/aegis.hpp>

#include <gtest/gtest.h>

TEST(FixedPointTest, DeltaTimeIsPositive)
{
    ae::fixed_t dt = ae::fixed_t(1) / 60;

    EXPECT_GT(dt, ae::fixed_t(0));
}

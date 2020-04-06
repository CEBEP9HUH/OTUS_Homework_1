#include "lib.h"
#include "gtest/gtest.h"

TEST(VERSION, IS_GT_0)
{
    ASSERT_GT(version(), 0);
}
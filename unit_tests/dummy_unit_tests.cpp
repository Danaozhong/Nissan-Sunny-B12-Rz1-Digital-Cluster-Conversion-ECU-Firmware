#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <random>
#include <algorithm>
#include <ctime> // for true random numbers

#include "version_info.hpp"

/** This is just a basic test to make sure the actual algorithm works as expected */
TEST(BasicTest, BasicTest) 
{
    EXPECT_EQ(app::get_app_name(), "Nissan Sunny Rz1 Digital Cluster Converter");
    
#if 0
    for (int i = 0; i < 100000; ++i)
    {
        EXPECT_EQ(..., ...);
        EXPECT_TRUE(...);
    }
#endif
}

#include "processing.h"
#include <gtest/gtest.h>
#include <sstream>
#include <vector>

TEST(OutTest, WritesIntegersToStream) {
    std::vector<int> input = {1, 2, 3};
    std::stringstream ss;

    AsDataFlow(input) | Out(ss);

    ASSERT_EQ(ss.str(), "123");
}

TEST(OutTest, WritesStringsToStream) {
    std::vector<std::string> input = {"ab", "cd"};
    std::stringstream ss;

    AsDataFlow(input) | Out(ss);

    ASSERT_EQ(ss.str(), "abcd");
}
#include "processing.h"
#include <gtest/gtest.h>
#include <vector>
#include <string>

TEST(AsVectorTest, ConvertsFlowOfIntsToVector) {
    std::vector<int> input = {4, 5, 6};

    auto result = AsDataFlow(input) | AsVector();

    ASSERT_EQ(result, std::vector<int>({4, 5, 6}));
}

TEST(AsVectorTest, ConvertsFlowOfStringsToVector) {
    std::vector<std::string> input = {"aa", "bb", "cc"};

    auto result = AsDataFlow(input) | AsVector();

    ASSERT_EQ(result, std::vector<std::string>({"aa", "bb", "cc"}));
}
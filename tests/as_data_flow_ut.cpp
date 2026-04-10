#include "processing.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <string>

using namespace testing;

TEST(AsDataFlowTest, ConvertsIntVectorToFlow) {
    std::vector<int> input = {1, 2, 3};

    auto flow = AsDataFlow(input);

    ASSERT_THAT(flow, ElementsAre(1, 2, 3));
}

TEST(AsDataFlowTest, ConvertsStringVectorToFlow) {
    std::vector<std::string> input = {"one", "two"};

    auto flow = AsDataFlow(input);

    ASSERT_THAT(flow, ElementsAre("one", "two"));
}
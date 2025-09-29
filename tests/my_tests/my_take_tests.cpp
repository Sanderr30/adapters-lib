#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>


TEST(MyTakeTest, TakeSomeElements) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(data) | Take(3) | AsVector();
    EXPECT_THAT(result, ::testing::ElementsAre(1, 2, 3));
}


TEST(MyTakeTest, TakeZeroElements) {
    std::vector<int> data = {1, 2, 3};
    auto result = AsDataFlow(data) | Take(0) | AsVector();
    EXPECT_TRUE(result.empty());
}


TEST(MyTakeTest, TakeOnEmptyRange) {
    std::vector<int> data;
    auto result = AsDataFlow(data) | Take(3) | AsVector();
    EXPECT_TRUE(result.empty());
}


TEST(MyTakeTest, TakeWithStrings) {
    std::vector<std::string> data = {"a", "b", "c"};
    auto result = AsDataFlow(data) | Take(2) | AsVector();
    EXPECT_THAT(result, ::testing::ElementsAre("a", "b"));
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>


TEST(MySkipTest, SkipSomeElements) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(data) | Skip(2) | AsVector();
    EXPECT_THAT(result, ::testing::ElementsAre(3, 4, 5));
}


TEST(MySkipTest, SkipAllElements) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(data) | Skip(5) | AsVector();
    EXPECT_TRUE(result.empty());
}


TEST(MySkipTest, SkipOnEmptyRange) {
    std::vector<int> data;
    auto result = AsDataFlow(data) | Skip(3) | AsVector();
    EXPECT_TRUE(result.empty());
}


TEST(MySkipTest, SkipWithStrings) {
    std::vector<std::string> data = {"a", "b", "c", "d"};
    auto result = AsDataFlow(data) | Skip(2) | AsVector();
    EXPECT_THAT(result, ::testing::ElementsAre("c", "d"));
}
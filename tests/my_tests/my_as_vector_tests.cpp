#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <string>


TEST(MyAsVectorTest, CollectToVector) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(data) | AsVector();
    EXPECT_EQ(result, data);
}

TEST(MyAsVectorTest, CollectEmptyVector) {
    std::vector<int> data = {};
    auto result = AsDataFlow(data) | AsVector();
    EXPECT_TRUE(result.empty());
}

TEST(MyAsVectorTest, CollectToStringVector) {
    std::vector<std::string> data = {"hello", "world"};
    auto result = AsDataFlow(data) | AsVector();
    EXPECT_EQ(result, data);
}
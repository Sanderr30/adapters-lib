#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <string>


TEST(MyDropNulloptTest, BasicUsage) {
    std::vector<std::optional<int>> data = {1, std::nullopt, 2, std::nullopt, 3};
    auto result = AsDataFlow(data) | DropNullopt() | AsVector()
    EXPECT_THAT(result, ::testing::ElementsAre(1, 2, 3));
}


TEST(MyDropNulloptTest, NoNullopt) {
    std::vector<std::optional<int>> data = {1, 2, 3};
    auto result = AsDataFlow(data) | DropNullopt() | AsVector();
    EXPECT_THAT(result, ::testing::ElementsAre(1, 2, 3));
}


TEST(MyDropNulloptTest, EmptyInput) {
    std::vector<std::optional<int>> data = {std::nullopt, std::nullopt};
    auto result = AsDataFlow(data) | DropNullopt() | AsVector();
    EXPECT_TRUE(result.empty());
}


TEST(MyDropNulloptTest, MixedTypes) {
    std::vector<std::optional<std::string>> data = {"hello", std::nullopt, "world", std::nullopt, "!"};
    auto result = AsDataFlow(data) | DropNullopt() | AsVector();
    EXPECT_THAT(result, ::testing::ElementsAre("hello", "world", "!"));
}
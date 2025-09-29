#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <string>


TEST(MySplitTest, SplitBySpace) {
    std::vector<std::stringstream> files(1);
    files[0] << "1 2 3 4 5 6 7 8 9 10";

    auto result = AsDataFlow(files) | Split(" ") | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre("1", "2", "3", "4", "5", "6", "7", "8", "9", "10"));
}


TEST(MySplitTest, SplitByNewLine) {
    std::vector<std::stringstream> files(1);
    files[0] << "line1\nline2\nline3\nline4";

    auto result = AsDataFlow(files) | Split("\n") | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre("line1", "line2", "line3", "line4"));
}


TEST(MySplitTest, EmptyFile) {
    std::vector<std::stringstream> files(1);
    files[0] << "";

    auto result = AsDataFlow(files) | Split(" ") | AsVector();

    EXPECT_TRUE(result.empty());
}


TEST(MySplitTest, SplitWithMultipleDelimiters) {
    std::vector<std::stringstream> files(1);
    files[0] << "1,2;3:4|5";

    auto result = AsDataFlow(files) | Split(",;:|") | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre("1", "2", "3", "4", "5"));
}



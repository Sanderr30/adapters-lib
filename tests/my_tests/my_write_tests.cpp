#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <string>


TEST(MyWriteTest, WriteWithDelimiter) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    std::ostringstream oss;

    AsDataFlow(data) | Write(oss, ',');

    EXPECT_EQ(oss.str(), "1,2,3,4,5,");
}


TEST(MyWriteTest, EmptyInput) {
    std::vector<int> data;
    std::ostringstream oss;

    AsDataFlow(data) | Write(oss, ',');

    EXPECT_EQ(oss.str(), "");
}


TEST(MyWriteTest, WriteWithNewLine) {
    std::vector<std::string> data = {"line1", "line2", "line3"};
    std::ostringstream oss;

    AsDataFlow(data) | Write(oss, '\n');

    EXPECT_EQ(oss.str(), "line1\nline2\nline3\n");
}
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <string>


TEST(MyAsDataFlowTest, BasicFlow) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto flow = AsDataFlow(data);

    std::vector<int> result;
    for (const auto& item : flow) {
        result.push_back(item);
    }

    EXPECT_EQ(result, data);
}


TEST(MyOutTest, OutputIntegersToStream) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    std::ostringstream oss;

    AsDataFlow(data) | Out(oss);

    EXPECT_EQ(oss.str(), "1\n2\n3\n4\n5\n");
}


TEST(MyOutTest, OutputStringsToStream) {
    std::vector<std::string> data = {"hello", "world"};
    std::ostringstream oss;

    AsDataFlow(data) | Out(oss);

    EXPECT_EQ(oss.str(), "hello\nworld\n");
}


TEST(MyOutTest, OutputEmptyStream) {
    std::vector<int> data = {};
    std::ostringstream oss;

    AsDataFlow(data) | Out(oss);

    EXPECT_EQ(oss.str(), "");
}
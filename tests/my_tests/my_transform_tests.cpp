#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <string>


TEST(MyTransformTest, MultiplyByTwo) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto flow = AsDataFlow(data) | Transform([](int x) { return x * 2; });

    std::vector<int> result;
    for (const auto& item : flow) {
        result.push_back(item);
    }

    EXPECT_EQ(result, (std::vector<int>{2, 4, 6, 8, 10}));
}


TEST(MyTransformTest, ToUpperCase) {
    std::vector<std::string> data = {"hello", "world"};
    auto flow = AsDataFlow(data) | Transform([](const std::string& str) {
        std::string upper;
        for (char c : str) {
            upper += std::toupper(c);
        }
        return upper;
    });

    std::vector<std::string> result;
    for (const auto& item : flow) {
        result.push_back(item);
    }

    EXPECT_EQ(result, (std::vector<std::string>{"HELLO", "WORLD"}));
}


TEST(MyTransformTest, TransformToString) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(data)
                  | Transform([](int x) { return std::to_string(x); })
                  | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre("1", "2", "3", "4", "5"));
}


TEST(MyTransformTest, TransformWithLambda) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(data)
                  | Transform([](int x) { return x + 1; })
                  | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre(2, 3, 4, 5, 6));
}
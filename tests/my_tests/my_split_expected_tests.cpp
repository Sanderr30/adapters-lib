#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <string>


struct Department {
    std::string name;

    bool operator==(const Department& other) const {
        return name == other.name;
    }
};

std::expected<Department, std::string> PDepart(const std::string& str) {
    if (str.empty()) {
        return std::unexpected("Department name is empty");
    }
    if (str.contains(' ')) {
        return std::unexpected("Department name contains space");
    }
    return Department{str};
}


TEST(MySplitExpectedTest, AllValidDepartments) {
    std::vector<std::stringstream> files(1);
    files[0] << "good-department|another-good-department";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") | Transform(PDepart) | SplitExpected(PDepart);

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow | AsVector();

    ASSERT_EQ(unexpected_file.str(), "");
    ASSERT_THAT(expected_result, testing::ElementsAre(Department{"good-department"}, Department{"another-good-department"}));
}

TEST(MySplitExpectedTest, AllInvalidDepartments) {
    std::vector<std::stringstream> files(1);
    files[0] << "bad department||another bad department";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") | Transform(PDepart) | SplitExpected(PDepart);

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow | AsVector();

    ASSERT_EQ(unexpected_file.str(), "Department name contains space.Department name is empty.Department name contains space.");
    ASSERT_TRUE(expected_result.empty());
}

TEST(MySplitExpectedTest, MixedValidAndInvalidDepartments) {
    std::vector<std::stringstream> files(1);
    files[0] << "good-department|bad department|another-good-department|";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") | Transform(PDepart) | SplitExpected(PDepart);

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow | AsVector();

    ASSERT_EQ(unexpected_file.str(), "Department name contains space.Department name is empty.");
    ASSERT_THAT(expected_result, testing::ElementsAre(Department{"good-department"}, Department{"another-good-department"}));
}
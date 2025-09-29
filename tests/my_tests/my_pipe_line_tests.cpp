#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <string>


TEST(MyPipelineTest, SplitTransformAndOut) {
    std::vector<std::stringstream> files(1);
    files[0] << "1 2 3 4 5";

    std::ostringstream oss;
    AsDataFlow(files)
        | Split(" ")
        | Transform([](const std::string& str) { return std::stoi(str) * 2; })
        | Out(oss);

    EXPECT_EQ(oss.str(), "2\n4\n6\n8\n10\n");
}


TEST(MyPipelineTest, SplitTransformToUpperAndOut) {
    std::vector<std::stringstream> files(1);
    files[0] << "hello world";

    std::ostringstream oss;
    AsDataFlow(files)
        | Split(" ")
        | Transform([](const std::string& str) {
              std::string upper;
              for (char c : str) {
                  upper += std::toupper(c);
              }
              return upper;
          })
        | Out(oss);

    EXPECT_EQ(oss.str(), "HELLO\nWORLD\n");
}


TEST(MyPipelineTest, TransformTwiceAndOut) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    std::ostringstream oss;

    AsDataFlow(data)
        | Transform([](int x) { return x * 2; })
        | Transform([](int x) { return x + 1; })
        | Out(oss);

    EXPECT_EQ(oss.str(), "3\n5\n7\n9\n11\n");
}


TEST(MyPipelineTest, FilterEvenNumbers) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto result = AsDataFlow(data)
                  | Filter([](int x) { return x % 2 == 0; })
                  | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre(2, 4, 6, 8, 10));
}


TEST(MyPipelineTest, FilterAndTransform) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto result = AsDataFlow(data)
                  | Filter([](int x) { return x % 2 == 0; })
                  | Transform([](int x) { return x * 10; })
                  | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre(20, 40, 60, 80, 100));
}


TEST(MyPipelineTest, FilterStringsByLength) {
    std::vector<std::string> data = {"short", "tiny", "verylongword", "medium"};
    auto result = AsDataFlow(data)
                  | Filter([](const std::string& str) { return str.size() > 5; })
                  | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre("verylongword", "medium"));
}


TEST(MyPipelineTest, FilterAndTransformStrings) {
    std::vector<std::string> data = {"short", "tiny", "verylongword", "medium"};
    auto result = AsDataFlow(data)
                  | Filter([](const std::string& str) { return str.size() > 5; })
                  | Transform([](const std::string& str) { return str + "!"; })
                  | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre("verylongword!", "medium!"));
}


TEST(MyPipelineTest, FilterWithSplit) {
    std::vector<std::stringstream> files(1);
    files[0] << "1 2 3 4 5 6 7 8 9 10";

    auto result = AsDataFlow(files)
                  | Split(" ")
                  | Filter([](const std::string& str) { return std::stoi(str) % 2 == 0; })
                  | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre("2", "4", "6", "8", "10"));
}


TEST(MyPipelineTest, FilterWithOpenFiles) {
    std::filesystem::create_directories("test_dir");
    std::ofstream("test_dir/file1.txt") << "apple banana";
    std::ofstream("test_dir/file2.txt") << "cherry date";

    auto result = Dir("test_dir", false)
                  | OpenFiles()
                  | Split(" ")
                  | Filter([](const std::string& word) { return word.size() > 5; })
                  | AsVector();

    EXPECT_THAT(result, ::testing::UnorderedElementsAre("banana", "cherry"));

    std::filesystem::remove_all("test_dir");
}


TEST(MyPipelineTest, FilterTransformAndWrite) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::ostringstream oss;

    AsDataFlow(data)
        | Filter([](int x) { return x % 2 == 0; })
        | Transform([](int x) { return x * 10; })
        | Write(oss, ',');

    EXPECT_EQ(oss.str(), "20,40,60,80,100,");
}


TEST(MyPipelineTest, SplitFilterTransformAndAsVector) {
    std::vector<std::stringstream> files(1);
    files[0] << "1 2 3 4 5 6 7 8 9 10";

    auto result = AsDataFlow(files)
                  | Split(" ")
                  | Filter([](const std::string& str) { return std::stoi(str) % 2 == 0; })
                  | Transform([](const std::string& str) { return std::to_string(std::stoi(str) * 2); })
                  | AsVector();

    EXPECT_THAT(result, ::testing::ElementsAre("4", "8", "12", "16", "20"));
}


TEST(MyPipelineTest, DirOpenFilesSplitAndWrite) {
    std::filesystem::create_directories("test_dir");
    std::ofstream("test_dir/file1.txt") << "apple banana";
    std::ofstream("test_dir/file2.txt") << "cherry date";

    std::ostringstream oss;
    Dir("test_dir", false)
        | OpenFiles()
        | Split(" ") 
        | Write(oss, '|');

    EXPECT_THAT(oss.str(), ::testing::HasSubstr("cherry|date|apple|banana|"));

    std::filesystem::remove_all("test_dir");
}


TEST(MyPipelineTest, TransformToUpperAndWrite) {
    std::vector<std::string> data = {"hello", "world"};
    std::ostringstream oss;

    AsDataFlow(data)
        | Transform([](const std::string& str) {
              std::string upper;
              for (char c : str) {
                  upper += std::toupper(c);
              }
              return upper;
          })
        | Write(oss, ' ');

    EXPECT_EQ(oss.str(), "HELLO WORLD ");
}


TEST(MyPipelineTest, FilterTransformAndOut) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    std::ostringstream oss;

    AsDataFlow(data)
        | Filter([](int x) { return x > 2; })
        | Transform([](int x) { return x * 3; })
        | Out(oss);

    EXPECT_EQ(oss.str(), "9\n12\n15\n");
}


TEST(MyPipelineTest, SplitFilterTransformWriteAndOut) {
    std::vector<std::stringstream> files(1);
    files[0] << "1 2 3 4 5 6 7 8 9 10";

    std::ostringstream oss;
    AsDataFlow(files)
        | Split(" ")
        | Filter([](const std::string& str) { return std::stoi(str) % 2 == 0; })
        | Transform([](const std::string& str) { return std::to_string(std::stoi(str) * 2); })
        | Write(oss, ',');

    EXPECT_EQ(oss.str(), "4,8,12,16,20,");
}


TEST(MyPipelineTest, DirOpenFilesSplitFilterTransformAndAsVector) {
    std::filesystem::create_directories("test_dir");
    std::ofstream("test_dir/file1.txt") << "apple banana";
    std::ofstream("test_dir/file2.txt") << "cherry date";

    auto result = Dir("test_dir", false)
                  | OpenFiles()
                  | Split(" ")
                  | Filter([](const std::string& word) { return word.size() > 5; })
                  | Transform([](const std::string& word) { return word + "!"; })
                  | AsVector();

    EXPECT_THAT(result, ::testing::UnorderedElementsAre("banana!", "cherry!"));

    std::filesystem::remove_all("test_dir");
}


TEST(MyPipelineTest, TransformTwice) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto flow = AsDataFlow(data)
                     | Transform([](int x) { return x * 2; })
                     | Transform([](int x) { return x + 1; });

    std::vector<int> result;
    for (const auto& item : flow) {
        result.push_back(item);
    }

    EXPECT_EQ(result, (std::vector<int>{3, 5, 7, 9, 11}));
}


TEST(MyPipelineTest, DirTransformAndOut) {
    std::filesystem::create_directories("test_dir");
    std::ofstream("test_dir/file1.txt");
    std::ofstream("test_dir/file2.txt");

    std::ostringstream oss;
    Dir("test_dir", false)
        | Transform([](const std::filesystem::path& p) { return p.filename().string(); })
        | Out(oss);

    std::istringstream iss(oss.str());
    
    std::vector<std::string> result((std::istream_iterator<std::string>(iss)),
                                    std::istream_iterator<std::string>());

    std::vector<std::string> expected = {"file1.txt", "file2.txt"};

    EXPECT_THAT(result, ::testing::UnorderedElementsAre("file2.txt", "file1.txt"));

    std::filesystem::remove_all("test_dir");
}
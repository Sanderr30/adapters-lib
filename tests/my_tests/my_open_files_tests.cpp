#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <string>


TEST(MyOpenFilesTest, BasicFileOpening) {
    std::filesystem::create_directories("test_dir");
    std::ofstream("test_dir/file1.txt") << "hello";
    std::ofstream("test_dir/file2.txt") << "world";
    std::ofstream("test_dir/file3.txt") << "test";

    size_t total_size = 0;
    for (auto& file : Dir("test_dir", false) | OpenFiles()) {
        std::string content;
        file >> content;
        total_size += content.size();
    }

    EXPECT_EQ(total_size, 14);

    std::filesystem::remove_all("test_dir");
}


TEST(MyOpenFilesTest, FailedOpenHandling) {
    std::filesystem::create_directory("test_fail_dir");
    std::ofstream("test_fail_dir/good1.txt");
    std::ofstream("test_fail_dir/good2.txt");
    std::filesystem::path p = std::filesystem::path("test_fail_dir/bad.txt");
    std::ofstream(p).put('x');
    std::filesystem::permissions(p, std::filesystem::perms::none);

    size_t opened_files = 0;
    for (auto& file : Dir("test_fail_dir", false) | OpenFiles()) {
        opened_files++;
    }

    EXPECT_EQ(opened_files, 2);

    std::filesystem::permissions(p, std::filesystem::perms::all);
    std::filesystem::remove_all("test_fail_dir");
}


TEST(MyOpenFilesTest, ReadFileContents) {
    std::filesystem::create_directories("test_dir");
    std::ofstream("test_dir/file1.txt") << "file1_content";
    std::ofstream("test_dir/file2.txt") << "file2_content";

    std::vector<std::string> contents;
    for (auto& file : Dir("test_dir", false) | OpenFiles()) {
        std::string content;
        file >> content;
        contents.push_back(content);
    }

    EXPECT_THAT(contents, ::testing::UnorderedElementsAre("file1_content", "file2_content"));

    std::filesystem::remove_all("test_dir");
}


TEST(MyOpenFilesTest, EmptyDirectory) {
    std::filesystem::create_directory("empty_dir");
    size_t opened_files = 0;
    for (auto& file : Dir("empty_dir", false) | OpenFiles()) {
        opened_files++;
    }

    EXPECT_EQ(opened_files, 0);

    std::filesystem::remove_all("empty_dir");
}
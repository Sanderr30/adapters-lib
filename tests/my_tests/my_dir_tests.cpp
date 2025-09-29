#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <string>


TEST(MyDirTest, NonRecursiveIteration) {
    std::filesystem::create_directories("test_dir/subdir");
    std::ofstream("test_dir/file1.txt");
    std::ofstream("test_dir/file2.txt");
    std::ofstream("test_dir/subdir/file3.txt");

    Dir dir("test_dir", false);
    size_t count = 0;

    for (const auto& path : dir) {
        EXPECT_TRUE(std::filesystem::exists(path));
        count++;
    }

    EXPECT_EQ(count, 3);

    std::filesystem::remove_all("test_dir");
}


TEST(MyDirTest, RecursiveIteration) {
    std::filesystem::create_directories("test_dir/subdir");
    std::ofstream("test_dir/file1.txt");
    std::ofstream("test_dir/file2.txt");
    std::ofstream("test_dir/subdir/file3.txt");

    Dir dir("test_dir", true);
    size_t count = 0;

    for (const auto& path : dir) {
        EXPECT_TRUE(std::filesystem::exists(path));
        count++;
    }

    EXPECT_EQ(count, 4);

    std::filesystem::remove_all("test_dir");
}


TEST(MyDirTest, EmptyDirectory) {
    std::filesystem::create_directory("empty_dir");
    Dir dir("empty_dir", false);

    EXPECT_EQ(dir.begin(), dir.end());

    std::filesystem::remove_all("empty_dir");
}
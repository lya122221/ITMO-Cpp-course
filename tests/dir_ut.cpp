#include "processing.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <filesystem>
#include <fstream>
#include <vector>

using namespace testing;

TEST(DirTest, ReturnsFilesFromDirectory) {
    std::filesystem::create_directory("dir_test_data");
    std::ofstream("dir_test_data/a.txt");
    std::ofstream("dir_test_data/b.txt");

    auto result = Dir("dir_test_data", false) | AsVector();

    ASSERT_THAT(result, SizeIs(2));

    std::filesystem::remove_all("dir_test_data");
}

TEST(DirTest, ReturnsEmptyForEmptyDirectory) {
    std::filesystem::create_directory("empty_dir_test");

    auto result = Dir("empty_dir_test", false) | AsVector();

    ASSERT_TRUE(result.empty());

    std::filesystem::remove_all("empty_dir_test");
}

TEST(DirTest, ReturnsFilesRecursively) {
    std::filesystem::create_directories("dir_recursive_test/subdir");
    std::ofstream("dir_recursive_test/root.txt");
    std::ofstream("dir_recursive_test/subdir/nested.txt");

    auto result = Dir("dir_recursive_test", true) | AsVector();

    ASSERT_THAT(result, SizeIs(2));

    std::filesystem::remove_all("dir_recursive_test");
}
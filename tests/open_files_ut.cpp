#include "processing.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <vector>

TEST(OpenFilesTest, OpensSingleFile) {
    std::ofstream("file1.txt") << "hello";

    std::vector<std::filesystem::path> files = {"file1.txt"};
    auto flow = AsDataFlow(files) | OpenFiles();

    auto it = flow.begin();
    auto end = flow.end();

    ASSERT_NE(it, end);
    ASSERT_TRUE((*it).is_open());

    std::filesystem::remove("file1.txt");
}

TEST(OpenFilesTest, OpensSeveralFiles) {
    std::ofstream("file1.txt") << "a";
    std::ofstream("file2.txt") << "b";

    std::vector<std::filesystem::path> files = {"file1.txt", "file2.txt"};
    auto flow = AsDataFlow(files) | OpenFiles();

    auto it = flow.begin();
    auto end = flow.end();

    ASSERT_NE(it, end);
    ASSERT_TRUE((*it).is_open());
    ++it;

    ASSERT_NE(it, end);
    ASSERT_TRUE((*it).is_open());
    ++it;

    ASSERT_EQ(it, end);

    std::filesystem::remove("file1.txt");
    std::filesystem::remove("file2.txt");
}
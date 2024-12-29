#include "FileComparator.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

TEST(FileComparatorTests, TestScanDirectory) {
    const std::string testDir = "test_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/sample.txt") << "This is a test file.";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_FALSE(files.empty());
    ASSERT_EQ(files[0].name, "sample.txt");

    // Clean up (optional)
    fs::remove_all(testDir);
}

TEST(FileComparatorTests, TestCompareFiles) {
    FileComparator::FileInfo file1{"path1", "name1", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "name1", 100, "hash1"};
    ASSERT_TRUE(FileComparator::compareFiles(file1, file2));
}

TEST(FileComparatorTests, TestEmptyDirectory) {
    const std::string testDir = "empty_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_TRUE(files.empty());

    // Clean up
    fs::remove_all(testDir);
}

TEST(FileComparatorTests, TestMultipleFiles) {
    const std::string testDir = "multi_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file1.txt") << "Content1";
        std::ofstream(testDir + "/file2.txt") << "Content2";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);

    // Clean up
    fs::remove_all(testDir);
}

TEST(FileComparatorTests, TestFileHash) {
    FileComparator::FileInfo file1{"path1", "name1", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "name2", 100, "hash2"};
    ASSERT_NE(file1.hash, file2.hash);
}

TEST(FileComparatorTests, TestSameSizeDifferentHash) {
    FileComparator::FileInfo file1{"path1", "name1", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "name2", 100, "hash2"};
    ASSERT_FALSE(FileComparator::compareFiles(file1, file2));
}

TEST(FileComparatorTests, TestSameHashSameContent) {
    FileComparator::FileInfo file1{"path1", "name1", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "name2", 100, "hash1"};
    ASSERT_TRUE(FileComparator::compareFiles(file1, file2));
}

TEST(FileComparatorTests, TestFileNameComparison) {
    FileComparator::FileInfo file1{"path1", "file1.txt", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "file2.txt", 100, "hash1"};
    ASSERT_NE(file1.name, file2.name);
}

TEST(FileComparatorTests, TestDirectoryScanWithSubdirs) {
    const std::string rootDir = "root_directory";
    const std::string subDir = rootDir + "/subdir";

    if (!fs::exists(rootDir)) {
        fs::create_directory(rootDir);
        fs::create_directory(subDir);
        std::ofstream(rootDir + "/file1.txt") << "RootFile";
        std::ofstream(subDir + "/file2.txt") << "SubFile";
    }

    auto files = FileComparator::scanDirectory(rootDir);
    ASSERT_EQ(files.size(), 2);

    // Clean up
    fs::remove_all(rootDir);
}

TEST(FileComparatorTests, TestInvalidDirectory) {
    const std::string invalidDir = "invalid_directory";

    auto files = FileComparator::scanDirectory(invalidDir);
    ASSERT_TRUE(files.empty());
}

// Additional tests
TEST(FileComparatorTests, TestDuplicateFiles) {
    const std::string testDir = "dup_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file1.txt") << "DuplicateContent";
        std::ofstream(testDir + "/file2.txt") << "DuplicateContent";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);
    ASSERT_TRUE(FileComparator::compareFiles(files[0], files[1]));

    // Clean up
    fs::remove_all(testDir);
}

TEST(FileComparatorTests, TestFileExtensions) {
    FileComparator::FileInfo file1{"path1", "file1.txt", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "file2.csv", 100, "hash1"};
    ASSERT_NE(file1.name.substr(file1.name.find_last_of('.')),
              file2.name.substr(file2.name.find_last_of('.')));
}

TEST(FileComparatorTests, TestHiddenFiles) {
    const std::string testDir = "hidden_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/.hidden.txt") << "HiddenContent";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 1);
    ASSERT_EQ(files[0].name, ".hidden.txt");

    // Clean up
    fs::remove_all(testDir);
}

TEST(FileComparatorTests, TestLargeFiles) {
    FileComparator::FileInfo file1{"path1", "large1.bin", 1000000, "hash1"};
    FileComparator::FileInfo file2{"path2", "large2.bin", 1000000, "hash1"};
    ASSERT_TRUE(FileComparator::compareFiles(file1, file2));
}

TEST(FileComparatorTests, TestEmptyFiles) {
    const std::string testDir = "empty_files";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file1.txt");
        std::ofstream(testDir + "/file2.txt");
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);
    ASSERT_TRUE(FileComparator::compareFiles(files[0], files[1]));

    // Clean up
    fs::remove_all(testDir);
}

TEST(FileComparatorTests, TestNestedDirectoryStructure) {
    const std::string rootDir = "nested_directory";
    const std::string subDir1 = rootDir + "/subdir1";
    const std::string subDir2 = rootDir + "/subdir2";

    if (!fs::exists(rootDir)) {
        fs::create_directory(rootDir);
        fs::create_directory(subDir1);
        fs::create_directory(subDir2);
        std::ofstream(subDir1 + "/file1.txt") << "Content1";
        std::ofstream(subDir2 + "/file2.txt") << "Content2";
    }

    auto files = FileComparator::scanDirectory(rootDir);
    ASSERT_EQ(files.size(), 2);

    // Clean up
    fs::remove_all(rootDir);
}


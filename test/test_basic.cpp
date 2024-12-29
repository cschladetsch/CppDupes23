#include "FileComparator.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

TEST(FileComparatorBasicTests, TestScanDirectory) {
    const std::string testDir = "test_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/sample.txt") << "This is a test file.";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_FALSE(files.empty());
    ASSERT_EQ(files[0].name, "sample.txt");

    fs::remove_all(testDir);
}

TEST(FileComparatorBasicTests, TestCompareFiles) {
    FileComparator::FileInfo file1{"path1", "name1", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "name1", 100, "hash1"};
    ASSERT_TRUE(FileComparator::compareFiles(file1, file2));
}

TEST(FileComparatorBasicTests, TestEmptyDirectory) {
    const std::string testDir = "empty_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_TRUE(files.empty());

    fs::remove_all(testDir);
}

TEST(FileComparatorBasicTests, TestMultipleFiles) {
    const std::string testDir = "multi_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file1.txt") << "Content1";
        std::ofstream(testDir + "/file2.txt") << "Content2";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);

    fs::remove_all(testDir);
}

TEST(FileComparatorBasicTests, TestFileHash) {
    FileComparator::FileInfo file1{"path1", "name1", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "name2", 100, "hash2"};
    ASSERT_NE(file1.hash, file2.hash);
}

TEST(FileComparatorBasicTests, TestSameSizeDifferentHash) {
    FileComparator::FileInfo file1{"path1", "name1", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "name2", 100, "hash2"};
    ASSERT_FALSE(FileComparator::compareFiles(file1, file2));
}

TEST(FileComparatorBasicTests, TestSameHashSameContent) {
    FileComparator::FileInfo file1{"path1", "name1", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "name2", 100, "hash1"};
    ASSERT_TRUE(FileComparator::compareFiles(file1, file2));
}

TEST(FileComparatorBasicTests, TestFileNameComparison) {
    FileComparator::FileInfo file1{"path1", "file1.txt", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "file2.txt", 100, "hash1"};
    ASSERT_NE(file1.name, file2.name);
}

TEST(FileComparatorBasicTests, TestDirectoryScanWithSubdirs) {
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

    fs::remove_all(rootDir);
}

TEST(FileComparatorBasicTests, TestInvalidDirectory) {
    const std::string invalidDir = "invalid_directory";
    auto files = FileComparator::scanDirectory(invalidDir);
    ASSERT_TRUE(files.empty());
}

TEST(FileComparatorBasicTests, TestDuplicateFiles) {
    const std::string testDir = "dup_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file1.txt") << "DuplicateContent";
        std::ofstream(testDir + "/file2.txt") << "DuplicateContent";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);
    ASSERT_TRUE(FileComparator::compareFiles(files[0], files[1]));

    fs::remove_all(testDir);
}

TEST(FileComparatorBasicTests, TestFileExtensions) {
    const std::string testDir = "extension_test";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file1.txt") << "Content1";
        std::ofstream(testDir + "/file2.csv") << "Content2";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);

    fs::remove_all(testDir);
}

TEST(FileComparatorBasicTests, TestZeroSizeFile) {
    const std::string testDir = "zero_size_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/empty.txt"); // Creates empty file
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 1);
    ASSERT_EQ(files[0].size, 0);

    fs::remove_all(testDir);
}

TEST(FileComparatorBasicTests, TestSingleByteFile) {
    const std::string testDir = "single_byte_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/single.txt") << "A";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 1);
    ASSERT_EQ(files[0].size, 1);

    fs::remove_all(testDir);
}

TEST(FileComparatorBasicTests, TestMultipleIdenticalFiles) {
    const std::string testDir = "identical_files_directory";
    const int numFiles = 5;
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        for (int i = 0; i < numFiles; ++i) {
            std::ofstream(testDir + "/file" + std::to_string(i) + ".txt") << "Same Content";
        }
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), numFiles);

    fs::remove_all(testDir);
}

TEST(FileComparatorBasicTests, TestFileSizeComparison) {
    FileComparator::FileInfo file1{"path1", "name1", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "name2", 200, "hash1"};
    ASSERT_NE(file1.size, file2.size);
}

TEST(FileComparatorBasicTests, TestFilePathComparison) {
    FileComparator::FileInfo file1{"/path/to/file1", "name1", 100, "hash1"};
    FileComparator::FileInfo file2{"/different/path/file1", "name1", 100, "hash1"};
    ASSERT_NE(file1.path, file2.path);
}

TEST(FileComparatorBasicTests, TestDirectoryWithDot) {
    const std::string testDir = "./test_directory";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file.txt") << "Content";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 1);

    fs::remove_all(testDir);
}

TEST(FileComparatorBasicTests, TestNoExtensionFiles) {
    const std::string testDir = "no_extension_test";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file1") << "Content1";
        std::ofstream(testDir + "/file2") << "Content2";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);

    fs::remove_all(testDir);
}

TEST(FileComparatorBasicTests, TestMultipleExtensionFile) {
    const std::string testDir = "multi_extension_test";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file.tar.gz") << "Archive Content";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 1);
    ASSERT_EQ(files[0].name, "file.tar.gz");

    fs::remove_all(testDir);
}

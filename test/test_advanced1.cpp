#include "FileComparator.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

namespace fs = std::filesystem;

TEST(FileComparatorAdvancedTests, TestSymbolicLinks) {
    const std::string testDir = "symlink_directory";
    const std::string targetDir = "target_directory";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        fs::create_directory(targetDir);
        std::ofstream(targetDir + "/target.txt") << "Target content";
        fs::create_symlink(targetDir + "/target.txt", testDir + "/link.txt");
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 1);
    ASSERT_EQ(files[0].name, "link.txt");

    fs::remove_all(testDir);
    fs::remove_all(targetDir);
}

TEST(FileComparatorAdvancedTests, TestDirectoryPermissions) {
    const std::string testDir = "permission_directory";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/test.txt") << "Test content";
        fs::permissions(testDir, fs::perms::none);
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_TRUE(files.empty());

    fs::permissions(testDir, fs::perms::all);
    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests, TestSpecialCharactersInFilenames) {
    const std::string testDir = "special_chars_directory";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file@#$%.txt") << "Special chars";
        std::ofstream(testDir + "/file spaces.txt") << "Spaces in name";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests, TestDeepNestedStructure) {
    const std::string rootDir = "deep_nested";
    std::string currentPath = rootDir;
    
    if (!fs::exists(rootDir)) {
        fs::create_directories(rootDir);
        for (int i = 0; i < 5; ++i) {
            currentPath += "/level" + std::to_string(i);
            fs::create_directories(currentPath);
        }
        std::ofstream(currentPath + "/deep_file.txt") << "Deep content";
    }

    auto files = FileComparator::scanDirectory(rootDir);
    ASSERT_EQ(files.size(), 1);
    ASSERT_EQ(files[0].name, "deep_file.txt");

    fs::remove_all(rootDir);
}

TEST(FileComparatorAdvancedTests, TestMaxPathLength) {
    const std::string testDir = "max_path_directory";
    std::string longFilename(255, 'a'); // Max filename length in many filesystems
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/" + longFilename) << "Long filename content";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 1);
    ASSERT_EQ(files[0].name.length(), 255);

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests, TestInternationalCharacters) {
    const std::string testDir = "international_directory";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/файл.txt") << "Russian filename";
        std::ofstream(testDir + "/文件.txt") << "Chinese filename";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);

    fs::remove_all(testDir);
}

/*
TEST(FileComparatorAdvancedTests, TestRecursiveSymlinks) {
    const std::string testDir = "recursive_symlink_directory";
    const std::string subDir = testDir + "/subdir";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        fs::create_directory(subDir);
        fs::create_symlink(testDir, subDir + "/recursive");
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_TRUE(files.empty());

    fs::remove_all(testDir);
}
*/

TEST(FileComparatorAdvancedTests, TestTemporaryFiles) {
    const std::string testDir = "temp_directory";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/~tempfile.txt") << "Temporary content";
        std::ofstream(testDir + "/.swp") << "Swap file";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests, TestFileModification) {
    const std::string testDir = "mod_directory";
    const std::string filename = testDir + "/mod_file.txt";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(filename) << "Initial content";
    }

    auto files1 = FileComparator::scanDirectory(testDir);
    std::ofstream(filename, std::ios::trunc) << "Modified"; // Use trunc to ensure file is rewritten
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Ensure modification is complete
    auto files2 = FileComparator::scanDirectory(testDir);
    
    ASSERT_NE(files1[0].hash, files2[0].hash); // Compare hashes instead of sizes

    fs::remove_all(testDir);
}

/*
TEST(FileComparatorAdvancedTests, TestNestedSymlinks) {
    const std::string testDir = "nested_symlinks";
    const std::string subDir = testDir + "/sub";
    const std::string targetFile = testDir + "/target.txt";
    const std::string link1 = subDir + "/link1";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        fs::create_directory(subDir);
        std::ofstream(targetFile) << "Target content";
        fs::create_symlink(targetFile, link1);
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2); // Original file and symlink

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests, TestCircularSymlinks) {
    const std::string testDir = "circular_symlinks";
    const std::string link1 = testDir + "/link1";
    const std::string link2 = testDir + "/link2";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        fs::create_symlink(link2, link1);
        fs::create_symlink(link1, link2);
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_TRUE(files.empty()); // Should handle circular symlinks gracefully

    fs::remove_all(testDir);
}
*/

TEST(FileComparatorAdvancedTests, TestUnicodeFilenames) {
    const std::string testDir = "unicode_directory";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/🌟.txt") << "Star";
        std::ofstream(testDir + "/👍.txt") << "Thumbs up";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 2);

    fs::remove_all(testDir);
}


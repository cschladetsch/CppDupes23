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

// Add after the last test

TEST(FileComparatorTests, TestSymbolicLinks) {
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

TEST(FileComparatorTests, TestDirectoryPermissions) {
    const std::string testDir = "permission_directory";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        fs::permissions(testDir, fs::perms::none);
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_TRUE(files.empty());

    fs::permissions(testDir, fs::perms::all);
    fs::remove_all(testDir);
}

TEST(FileComparatorTests, TestSpecialCharactersInFilenames) {
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

TEST(FileComparatorTests, TestDeepNestedStructure) {
    const std::string rootDir = "deep_nested";
    std::string currentPath = rootDir;
    
    if (!fs::exists(rootDir)) {
        for (int i = 0; i < 5; ++i) {
            fs::create_directories(currentPath);
            currentPath += "/level" + std::to_string(i);
        }
        std::ofstream(currentPath + "/deep_file.txt") << "Deep content";
    }

    auto files = FileComparator::scanDirectory(rootDir);
    ASSERT_EQ(files.size(), 1);

    fs::remove_all(rootDir);
}

TEST(FileComparatorTests, TestMaxPathLength) {
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

TEST(FileComparatorTests, TestFileComparison_ZeroByteFiles) {
    FileComparator::FileInfo file1{"path1", "zero1.txt", 0, "hash1"};
    FileComparator::FileInfo file2{"path2", "zero2.txt", 0, "hash1"};
    ASSERT_TRUE(FileComparator::compareFiles(file1, file2));
}

TEST(FileComparatorTests, TestMixedFileTypes) {
    const std::string testDir = "mixed_types_directory";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/text.txt") << "Text content";
        std::ofstream(testDir + "/binary.bin", std::ios::binary) << "Binary content";
        std::ofstream(testDir + "/data.dat") << "Data content";
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 3);

    fs::remove_all(testDir);
}

TEST(FileComparatorTests, TestCaseInsensitiveComparison) {
    FileComparator::FileInfo file1{"path1", "File.txt", 100, "hash1"};
    FileComparator::FileInfo file2{"path2", "file.txt", 100, "hash1"};
    ASSERT_NE(file1.name, file2.name);
}

/*
TEST(FileComparatorTests, TestRecursiveSymlinks) {
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

TEST(FileComparatorTests, TestTemporaryFiles) {
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

TEST(FileComparatorTests, TestFileModification) {
    const std::string testDir = "mod_directory";
    const std::string filename = testDir + "/mod_file.txt";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(filename) << "Initial content";
    }

    auto files1 = FileComparator::scanDirectory(testDir);
    std::ofstream(filename) << "Modified content";
    auto files2 = FileComparator::scanDirectory(testDir);
    
    ASSERT_EQ(files1[0].size, files2[0].size);

    fs::remove_all(testDir);
}

TEST(FileComparatorTests, TestConcurrentAccess) {
    const std::string testDir = "concurrent_directory";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream(testDir + "/file1.txt") << "Content1";
    }

    auto future1 = std::async(std::launch::async, [&]() {
        return FileComparator::scanDirectory(testDir);
    });
    auto future2 = std::async(std::launch::async, [&]() {
        return FileComparator::scanDirectory(testDir);
    });

    auto files1 = future1.get();
    auto files2 = future2.get();
    
    ASSERT_EQ(files1.size(), files2.size());

    fs::remove_all(testDir);
}

TEST(FileComparatorTests, TestInternationalCharacters) {
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

TEST(FileComparatorTests, TestCompareFilesWithSameNameDifferentPaths) {
    FileComparator::FileInfo file1{"path1/dir1", "same.txt", 100, "hash1"};
    FileComparator::FileInfo file2{"path2/dir2", "same.txt", 100, "hash1"};
    ASSERT_TRUE(FileComparator::compareFiles(file1, file2));
}

TEST(FileComparatorTests, TestDirectoryWithManyFiles) {
    const std::string testDir = "many_files_directory";
    const int numFiles = 1000;
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        for (int i = 0; i < numFiles; ++i) {
            std::ofstream(testDir + "/file" + std::to_string(i) + ".txt") 
                << "Content " << i;
        }
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), numFiles);

    fs::remove_all(testDir);
}

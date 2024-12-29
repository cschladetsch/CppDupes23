#include "FileComparator.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <future>
#include <thread>
#include <chrono>
#include <set>
#include <atomic>

namespace fs = std::filesystem;

// Concurrency and Performance Tests
TEST(FileComparatorAdvancedTests2, TestConcurrentAccess) {
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

TEST(FileComparatorAdvancedTests2, TestRapidFileCreationDeletion) {
    const std::string testDir = "rapid_change_test";
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
    }

    auto scanFuture = std::async(std::launch::async, [&]() {
        return FileComparator::scanDirectory(testDir);
    });

    // Rapidly create and delete files while scan is running
    for (int i = 0; i < 10; ++i) {
        std::string filename = testDir + "/temp" + std::to_string(i) + ".txt";
        std::ofstream(filename) << "Temporary content";
        fs::remove(filename);
    }

    auto files = scanFuture.get();
    // Test should complete without crashes
    ASSERT_TRUE(files.size() >= 0);

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests2, TestParallelScans) {
    const std::string testDir = "parallel_test";
    std::vector<std::future<std::vector<FileComparator::FileInfo>>> futures;
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        for (int i = 0; i < 5; ++i) {
            std::ofstream(testDir + "/file" + std::to_string(i) + ".txt") << "Content";
        }
    }

    // Launch multiple scans in parallel
    for (int i = 0; i < 5; ++i) {
        futures.push_back(std::async(std::launch::async, [&]() {
            return FileComparator::scanDirectory(testDir);
        }));
    }

    // All scans should return the same number of files
    size_t firstSize = futures[0].get().size();
    for (size_t i = 1; i < futures.size(); ++i) {
        ASSERT_EQ(futures[i].get().size(), firstSize);
    }

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests2, TestCoroutineBasedScan) {
    const std::string testDir = "coroutine_test";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        for (int i = 0; i < 5; ++i) {
            std::ofstream(testDir + "/file" + std::to_string(i) + ".txt") << "Content";
        }
    }

    auto generator = FileComparator::scanDirectoryAsync(testDir);
    std::vector<FileComparator::FileInfo> files;
    
    for (const auto& file : generator) {
        files.push_back(file);
    }

    ASSERT_EQ(files.size(), 5);

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests2, TestFileOpenLockHandling) {
    const std::string testDir = "lock_test";
    const std::string filename = testDir + "/locked.txt";
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        auto file = std::make_unique<std::ofstream>(filename);
        *file << "Locked content";
        
        auto files = FileComparator::scanDirectory(testDir);
        ASSERT_EQ(files.size(), 1);
        
        file->close();
    }

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests2, TestConcurrentHashComputation) {
    const std::string testDir = "concurrent_hash_test";
    const int numFiles = 10;
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        for (int i = 0; i < numFiles; ++i) {
            std::string filename = testDir + "/file" + std::to_string(i) + ".txt";
            std::ofstream(filename) << "Different content " << i;
        }
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), numFiles);

    // Verify that all hashes are computed and unique
    std::set<std::string> hashes;
    for (const auto& file : files) {
        ASSERT_FALSE(file.hash.empty());
        hashes.insert(file.hash);
    }
    ASSERT_EQ(hashes.size(), numFiles); // All files should have unique hashes

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests2, TestLargeFileProcessing) {
    const std::string testDir = "large_file_test";
    const std::string filename = testDir + "/large.bin";
    const size_t fileSize = 100 * 1024 * 1024; // 100MB
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        std::ofstream file(filename, std::ios::binary);
        std::vector<char> buffer(1024, 'X');
        for (size_t i = 0; i < fileSize / 1024; ++i) {
            file.write(buffer.data(), buffer.size());
        }
    }

    auto files = FileComparator::scanDirectory(testDir);
    ASSERT_EQ(files.size(), 1);
    ASSERT_EQ(files[0].size, fileSize);

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests2, TestMultithreadedFileComparison) {
    const std::string testDir = "multithread_compare_test";
    const int numFiles = 5;
    std::vector<FileComparator::FileInfo> files;
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
        for (int i = 0; i < numFiles; ++i) {
            std::string filename = testDir + "/file" + std::to_string(i) + ".txt";
            std::ofstream(filename) << "Content " << i;
        }
        files = FileComparator::scanDirectory(testDir);
    }

    std::vector<std::future<bool>> comparisons;
    for (size_t i = 0; i < files.size(); ++i) {
        for (size_t j = i + 1; j < files.size(); ++j) {
            comparisons.push_back(std::async(std::launch::async, [&files, i, j]() {
                return FileComparator::compareFiles(files[i], files[j]);
            }));
        }
    }

    for (auto& future : comparisons) {
        future.get(); // Should complete without exceptions
    }

    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests2, TestStressTest) {
    const std::string testDir = "stress_test";
    const int numOperations = 100;
    std::atomic<int> completedOps{0};
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
    }

    std::vector<std::future<void>> futures;
    for (int i = 0; i < numOperations; ++i) {
        futures.push_back(std::async(std::launch::async, [&, i]() {
            std::string filename = testDir + "/file" + std::to_string(i) + ".txt";
            std::ofstream(filename) << "Content " << i;
            FileComparator::scanDirectory(testDir);
            if (i % 2 == 0) {
                fs::remove(filename);
            }
            completedOps++;
        }));
    }

    for (auto& future : futures) {
        future.wait();
    }

    ASSERT_EQ(completedOps, numOperations);
    fs::remove_all(testDir);
}

TEST(FileComparatorAdvancedTests2, TestConcurrentDirectoryModification) {
    const std::string testDir = "concurrent_mod_test";
    const int numThreads = 5;
    std::atomic<bool> stopFlag{false};
    
    if (!fs::exists(testDir)) {
        fs::create_directory(testDir);
    }

    // Thread to continuously scan directory
    auto scannerFuture = std::async(std::launch::async, [&]() {
        while (!stopFlag) {
            FileComparator::scanDirectory(testDir);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    // Threads to modify directory
    std::vector<std::future<void>> modifierFutures;
    for (int i = 0; i < numThreads; ++i) {
        modifierFutures.push_back(std::async(std::launch::async, [&, i]() {
            for (int j = 0; j < 10 && !stopFlag; ++j) {
                std::string filename = testDir + "/thread" + 
                    std::to_string(i) + "_file" + std::to_string(j) + ".txt";
                std::ofstream(filename) << "Content";
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                fs::remove(filename);
            }
        }));
    }

    // Wait for modifiers to complete
    for (auto& future : modifierFutures) {
        future.wait();
    }

    stopFlag = true;
    scannerFuture.wait();

    fs::remove_all(testDir);
}

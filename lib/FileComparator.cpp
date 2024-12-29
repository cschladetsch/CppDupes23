#include "FileComparator.hpp"
#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

namespace FileComparator {

std::vector<FileInfo> scanDirectory(const std::string& directory) {
    std::vector<FileInfo> files;

    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back({
                    entry.path().string(),
                    entry.path().filename().string(),
                    entry.file_size(),
                    "hash_placeholder"
                });
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }

    return files;
}

bool compareFiles(const FileInfo& file1, const FileInfo& file2) {
    return file1.hash == file2.hash;
}

} // namespace FileComparator


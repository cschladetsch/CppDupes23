#pragma once

#include <string>
#include <vector>

namespace FileComparator {

struct FileInfo {
    std::string path;
    std::string name;
    std::size_t size;
    std::string hash;
};

std::vector<FileInfo> scanDirectory(const std::string& directory);
bool compareFiles(const FileInfo& file1, const FileInfo& file2);

} // namespace FileComparator


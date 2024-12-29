#include "FileComparator.hpp"
#include <filesystem>
#include <fstream>
#include <array>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>

namespace fs = std::filesystem;

namespace FileComparator {

namespace {
    constexpr size_t BUFFER_SIZE = 8192;
    ThreadPool pool;  // Global thread pool

    std::string calculateHash(const char* data, size_t size) {
        constexpr uint64_t FNV_PRIME = 1099511628211ULL;
        constexpr uint64_t FNV_OFFSET = 14695981039346656037ULL;

        uint64_t hash = FNV_OFFSET;
        for (size_t i = 0; i < size; ++i) {
            hash ^= static_cast<uint64_t>(data[i]);
            hash *= FNV_PRIME;
        }

        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(16) << hash;
        return ss.str();
    }

    std::string getFileContent(const fs::path& path) {
        try {
            if (fs::is_symlink(path)) {
                return fs::read_symlink(path).string(); // Use symlink target as content
            }

            std::ifstream file(path, std::ios::binary);
            if (!file) return std::string();

            file.seekg(0, std::ios::end);
            size_t size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::string content(size, '\0');
            file.read(content.data(), size);
            return content;
        } catch (...) {
            return std::string();
        }
    }
}

Generator<FileInfo> scanDirectoryAsync(const std::string& directory) {
    try {
        const fs::path dir_path(directory);
        if (!fs::exists(dir_path)) co_return;

        std::vector<std::future<std::string>> hashFutures;
        std::vector<fs::path> paths;

        fs::recursive_directory_iterator dirIt(
            dir_path,
            fs::directory_options::follow_directory_symlink | 
            fs::directory_options::skip_permission_denied
        );

        for (const auto& entry : dirIt) {
            try {
                if (entry.is_regular_file() || entry.is_symlink()) {
                    fs::path filePath = entry.path();
                    paths.push_back(filePath);
                    hashFutures.push_back(computeHashAsync(filePath.string()));
                }
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error processing entry: " << e.what() << std::endl;
                continue;
            }
        }

        for (size_t i = 0; i < paths.size(); ++i) {
            try {
                size_t fileSize = fs::is_symlink(paths[i]) ? 
                    fs::read_symlink(paths[i]).string().length() :
                    fs::file_size(paths[i]);

                co_yield FileInfo{
                    paths[i].string(),
                    paths[i].filename().string(),
                    fileSize,
                    hashFutures[i].get()
                };
            } catch (const fs::filesystem_error& e) {
                std::cerr << "Error yielding file info: " << e.what() << std::endl;
                continue;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

std::vector<FileInfo> scanDirectory(const std::string& directory) {
    std::vector<FileInfo> results;
    auto generator = scanDirectoryAsync(directory);
    for (const auto& info : generator) {
        results.push_back(info);
    }
    return results;
}

std::future<std::string> computeHashAsync(const std::string& path) {
    return pool.enqueue([path]() {
        try {
            if (fs::is_symlink(path)) {
                // For symlinks, hash the target path
                std::string targetPath = fs::read_symlink(path).string();
                return calculateHash(targetPath.data(), targetPath.size());
            }

            std::string content = getFileContent(path);
            if (content.empty()) return std::string();

            return calculateHash(content.data(), content.size());
        } catch (...) {
            return std::string();
        }
    });
}

bool compareFiles(const FileInfo& file1, const FileInfo& file2) {
    if (file1.hash.empty() || file2.hash.empty()) {
        auto future1 = computeHashAsync(file1.path);
        auto future2 = computeHashAsync(file2.path);
        
        return future1.get() == future2.get();
    }
    return file1.hash == file2.hash;
}

} // namespace FileComparator

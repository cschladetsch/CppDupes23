#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

void compare_directories(const std::vector<std::string>& dirs, bool verbose, const std::string& log_file) {
    std::unordered_map<std::string, std::vector<fs::path>> file_hash_map;
    
    std::ofstream log_stream;
    if (!log_file.empty()) {
        log_stream.open(log_file);
        if (!log_stream.is_open()) {
            std::cerr << "Failed to open log file: " << log_file << std::endl;
            return;
        }
    }
    auto& output = log_file.empty() ? std::cout : log_stream;

    for (const auto& dir : dirs) {
        if (!fs::exists(dir) || !fs::is_directory(dir)) {
            output << "Invalid directory: " << dir << std::endl;
            continue;
        }

        for (fs::recursive_directory_iterator it(dir), end; it != end; ++it) {
            if (fs::is_regular_file(it->path())) {
                std::string filename = it->path().filename().string();
                file_hash_map[filename].push_back(it->path());
            }
        }
    }

    for (const auto& [filename, paths] : file_hash_map) {
        if (paths.size() > 1) {
            output << "Duplicate file: " << filename << " found in locations:" << std::endl;
            for (const auto& path : paths) {
                output << "  " << path << std::endl;
            }
        }
    }

    if (verbose) {
        output << "Comparison complete." << std::endl;
    }

    if (log_stream.is_open()) {
        log_stream.close();
    }
}

int main(int argc, char* argv[]) {
    std::vector<std::string> directories;
    std::string log_file;
    bool verbose = false;

    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Show help message")
            ("directories,d", po::value<std::vector<std::string>>(&directories)->multitoken(), "Directories to compare")
            ("log-file,l", po::value<std::string>(&log_file), "Log file for output")
            ("verbose,v", po::bool_switch(&verbose), "Enable verbose output");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        if (directories.empty()) {
            std::cerr << "Error: At least one directory must be specified." << std::endl;
            return 1;
        }

        compare_directories(directories, verbose, log_file);

    } catch (const po::error& ex) {
        std::cerr << "Error parsing options: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}


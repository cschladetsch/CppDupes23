#include <boost/program_options.hpp>
#include <rang.hpp>
#include <iostream>
#include <vector>

#include "rang.hpp"
#include "FileComparator.hpp"

namespace po = boost::program_options;

void processDirectories(const std::vector<std::string>& directories) {
    using namespace FileComparator;

    for (const auto& dir : directories) {
        std::cout << rang::fg::yellow << "Scanning directory: " << dir << rang::style::reset << "\n";
        auto files = scanDirectory(dir);

        for (const auto& file : files) {
            std::cout << rang::fg::cyan
                      << "File: " << file.path << ", Size: " << file.size
                      << ", Hash: " << file.hash << rang::style::reset << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Print help message")
        ("directories,d", po::value<std::vector<std::string>>()->multitoken(), "Directories to scan");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help") || vm.empty()) {
            std::cout << desc << "\n";
            return 0;
        }

        if (vm.count("directories")) {
            auto directories = vm["directories"].as<std::vector<std::string>>();
            processDirectories(directories);
        } else {
            std::cerr << rang::fg::red << "No directories provided.\nUse -d option to specify directories.\n"
                      << rang::style::reset;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << rang::fg::red << "Error: " << e.what() << rang::style::reset << "\n";
        return 1;
    }

    return 0;
}


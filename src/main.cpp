#include "FileComparator.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <vector>
#include <string>

namespace po = boost::program_options;

int main(int argc, char** argv) {
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message")
            ("directory,d", po::value<std::vector<std::string>>(), "directories to scan")
        ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 1;
        }

        if (vm.count("directory")) {
            const auto& directories = vm["directory"].as<std::vector<std::string>>();
            for (const auto& dir : directories) {
                std::cout << "Scanning directory: " << dir << "\n";
                auto files = FileComparator::scanDirectory(dir);
                for (const auto& file : files) {
                    std::cout << file.path << " (" << file.size << " bytes)\n";
                }
            }
        } else {
            std::cout << "No directories specified.\n";
            std::cout << desc << "\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

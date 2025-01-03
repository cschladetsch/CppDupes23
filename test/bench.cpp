#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <random>

namespace fs = boost::filesystem;

void generate_test_files(const std::string& base_dir, size_t dir_count, size_t files_per_dir, size_t max_file_size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist_size(1, max_file_size);
    std::uniform_int_distribution<> dist_char('a', 'z');

    for (size_t i = 0; i < dir_count; ++i) {
        fs::path dir = fs::path(base_dir) / ("dir" + std::to_string(i));
        fs::create_directories(dir);

        for (size_t j = 0; j < files_per_dir; ++j) {
            fs::path file = dir / ("file" + std::to_string(j) + ".txt");
            std::ofstream ofs(file.string(), std::ios::binary);

            size_t file_size = dist_size(gen);
            for (size_t k = 0; k < file_size; ++k) {
                ofs.put(dist_char(gen));
            }
        }
    }
}

int main() {
    const std::string base_dir = "./benchmark_data";
    const size_t dir_count = 10;          // Number of directories
    const size_t files_per_dir = 100000; // Number of files per directory
    const size_t max_file_size = 1024;   // Max file size in bytes (1 KB)

    std::cout << "Generating test data..." << std::endl;
    generate_test_files(base_dir, dir_count, files_per_dir, max_file_size);

    std::cout << "Benchmark data generated. Use the tool to compare these directories:" << std::endl;
    for (size_t i = 0; i < dir_count; ++i) {
        std::cout << base_dir + "/dir" + std::to_string(i) << std::endl;
    }

    std::cout << "Run the following command for the benchmark:" << std::endl;
    std::cout << "./fsf --directories ";
    for (size_t i = 0; i < dir_count; ++i) {
        std::cout << base_dir + "/dir" + std::to_string(i) << " ";
    }
    std::cout << "--verbose --log-file benchmark.log" << std::endl;

    return 0;
}


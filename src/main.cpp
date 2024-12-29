#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

// Corrected function signature
void processDirectories(const std::vector<std::string>& directories) {
    for (const auto& dir : directories) {
        std::cout << "Scanning directory: " << dir << "\n";
    }
}

int main(int argc, char** argv) {
    try {
        // Placeholder for command-line argument processing
        std::vector<std::string> directories = {"example_dir1", "example_dir2"};
        processDirectories(directories);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}


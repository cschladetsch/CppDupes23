# File System Comparator (fsf)

## Overview

`fsf` is a high-performance C++23 command-line tool designed to compare files across multiple directories. It supports flexible file comparison modes, detailed logging, and customizable verbosity levels, making it an ideal tool for file management and analysis tasks.

## Features

- **Multiple Comparison Modes**:
  - `all`: Compare all files
  - `different`: Show only different files
  - `same`: Show only identical files
  - `equal`: Alias for `same`
  - `unique`: Show unique files

- **Logging and Verbosity**:
  - Adjustable verbosity for detailed logging
  - Output logs to a specified file

- **Performance and Flexibility**:
  - Concurrent file processing
  - MD5-based file comparison
  - Supports multiple directories
  - Automatic time logging

## Prerequisites

- C++23 compatible compiler (GCC 10+)
- CMake 3.10+
- Boost libraries (Filesystem, Program Options)

## Installation

### Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libboost-filesystem-dev libboost-program-options-dev
```

On macOS (using Homebrew):
```bash
brew install cmake boost
```

### Building

```bash
# Clone the repository
git clone <repository-url>
cd fsf

# Build the project
mkdir build
cd build
cmake ..
make
```

## Usage

### Basic Usage

```bash
# Compare files in two directories
./fsf --directories dir1 dir2 --verbose

# Compare files in multiple directories
./fsf --directories dir1 dir2 dir3 --log-file output.log

# Show help
./fsf --help
```

### Modes

- `all`: Show all file comparisons
- `different`: Show only files that differ
- `same`: Show only identical files
- `equal`: Alias for `same`
- `unique`: Show files unique to specific directories

### Performance Measurement

```bash
# Run comparison with detailed performance logging
./fsf --directories dir1 dir2 --log-file performance.log --verbose
```

## Output

The tool generates detailed output about file similarities and differences. Verbosity can be adjusted, and logs can be directed to a file for later analysis.

## Contributing

1. Fork the repository.
2. Create your feature branch.
3. Commit your changes.
4. Push to the branch.
5. Create a Pull Request.

## License

MIT License.

## Contact

Christian Schladetsch
Email: christian.schladetsch@gmail.com


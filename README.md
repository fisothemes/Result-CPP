# Result-CPP

Result-CPP is a lightweight, header-only C++ library for error handling using the monadic pattern in C++. It provides a generic class called `result` that can store either a successful value or an error value as well as ways to extract and handle them.

## Features

- Simple and concise result type for error handling.
- Header-only library with no external dependencies.
- Supports chaining operations and handling errors.

## Getting Started

### Prerequisites

- C++17 compiler

### Usage

1. Include the `result.hpp` header in your C++ project.
2. Start using the `fst::result` type for handling success and error states.

### Example

```cpp
#include <iostream>
#include <fstream>
#include <string>

#include "fst/result.hpp"

int main() {
  // Example usage
  auto fileResult = openFile("example.txt")
                        .and_then(processFile)
                        .or_else([](const std::string& error) {
                          std::cerr << "Error: " << error << std::endl;
                          return fst::result<std::ifstream, std::string>(error)
                        })
                        .and_then([](std::ifstream& file) { return closeFile(file); });

  // Check if the file operations were successful
  if (!fileResult) {
    std::cerr << fileResult << std::endl;
  }

  return 0;
}
```

```cpp
// Function to open a file and return a result
fst::result<std::ifstream, std::string> openFile(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    return std::string("Failed to open file: " + filename);
  }
  return file;
}

// Function to close a file
fst::result<std::ifstream, std::string> closeFile(std::ifstream& file) {
  file.close();
  if (file.fail()) {
    return std::string("Error closing file");
  }
  return file;
}

// Function to process a file
fst::result<std::ifstream, std::string> processFile(std::ifstream& file) {
  // Perform file processing here
  if (!file.good()) {
    return std::string("Error while processing file");
  }
  return file;
}
```

# Result-CPP

Result-CPP is a lightweight, header-only C++ library that provides a result type for handling success and error states.

## Features

- Simple and concise result type for error handling.
- Header-only library with no external dependencies.
- Supports chaining operations and handling errors elegantly. (W-I-P)

## Getting Started (W-I-P)

### Prerequisites

- C++17 compiler

### Usage

1. Include the `result.hpp` header in your C++ project.
2. Start using the `fst::result` type for handling success and error states.

### Examples

```cpp
#include <iostream>
#include "fst/result.hpp"

int main() {
    // Creating a result with a success value
    fst::result<int, std::string> successResult(42);

    // Accessing success value
    if (successResult) {
        std::cout << "Success: " << *successResult << std::endl;
    }

    // Creating a result with an error value
    fst::result<int, std::string> errorResult("Error occurred");

    // Accessing error value
    if (!errorResult) {
        std::cout << "Error: " << errorResult.error() << std::endl;
    }

    return 0;
}
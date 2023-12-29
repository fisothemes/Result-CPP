# Result-CPP

Result-CPP is a lightweight, header-only C++ library for monadic error handling in C++ using a result type.

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
#include <string>
#include <limits>

#include "fst/result.hpp"

// Function that may fail and return a Result
fst::result<double, std::string> div(double a, double b) {
  if (b == 0.0)
    return std::string("Division by zero error");

  return a / b;
}

int main() {
  // Example 1: Successful Result | Output: Result 1 value: 5
  std::cout << "Result 1 value: " << div(10.0, 2.0) << std::endl; 

  // Example 2: Errored Result | Output: Result 2 error: Division by zero error
  std::cout << "Result 2 error: " << div(5.0, 0.0) << std::endl;

  // Example 3: Chaining Operations - using or_else
  //            | Output: Handling error: Division by zero error
  //                      Result 3: inf
  auto result3 = div(5.0, 0.0)
    .or_else([](const std::string& error) {
      std::cout << "Handling error: " << error << std::endl;
      return fst::result<double, std::string>(
          std::numeric_limits<double>::infinity());
    });

  std::cout << "Result 3: " << result3 << '\n';

  // Example 4: Chaining Operations - using and_then
  //            | Output: Handling success: 5
  //                      Result 4: 2.5
  auto result4 = div(10.0, 2.0)
    .and_then([](const double& value) {
      std::cout << "Handling success: " << value << std::endl;
      return div(value, 2.0);
    });

  std::cout << "Result 4: " << result4 << '\n';

  return 0;
}
```
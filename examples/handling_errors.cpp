#include <iostream>
#include <string>

#include "fst/result.hpp"

// Function that may fail and return a Result
fst::result<double, std::string> div(double a, double b) {
  if (b == 0.0)
    return std::string(
        "Division by zero error");  // Implicitly converts to std::string
  return a / b;                     // Implicitly converts to double
}

int main() {
  // Example 1: Successful Result
  std::cout << "Result 1 value: " << div(10.0, 2.0) << '\n';

  // Example 2: Errored Result
  std::cout << "Result 2 error: " << div(5.0, 0.0) << '\n';

  return 0;
}
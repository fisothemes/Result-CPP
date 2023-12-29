#include <iostream>
#include <limits>

#include "fst/result.hpp"

// Function that may fail and return a Result
fst::result<double, std::string> div(double a, double b) {
  if (b == 0.0)
    return std::string(
        "Division by zero error");  // Implicitly converts to std::string
  return a / b;                     // Implicitly converts to double
}

int main() {

  // Example using or_else
  auto result1 = div(5.0, 0.0)
    .or_else([](const std::string& error) {
      std::cout << "Handling error: " << error << std::endl;
      return fst::result<double, std::string>(
          std::numeric_limits<double>::infinity());
    });

  std::cout << "Result 1: " << result1 << '\n';

  // Example using and_then
  auto result2 = div(10.0, 2.0)
    .and_then([](const double& value) {
      std::cout << "Handling success: " << value << std::endl;
      return div(value, 2.0);
    });

  std::cout << "Result 2: " << result2 << '\n';
}
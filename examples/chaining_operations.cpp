#include <iostream>
#include <limits>

#include "fst/result.hpp"

// Function that may fail and return a Result
fst::result<double, std::string> div(double a, double b) {
  if (b == 0.0)
    return std::string("Division by zero error");  // Implicitly converts to std::string
  return a / b;  // Implicitly converts to double
}

int main() {
// Example 1: Chaining with `and_then` and `map`
auto result1 = div(12.0, 3.0)
                    .and_then([](const auto& x) {
                      return x > 0.0 
                          ? fst::result<double, std::string>(x * 2.0) 
                          : fst::result<double, std::string>("Negative result");
                    })
                    .map([](const auto& y) {
                      std::cout << "Result: " + std::to_string(y) << '\n';
                      return y*y;
                    });

std::cout << "Example 1: " << result1 << '\n';

// Example 2: Chaining with `or_else` and `map_error`
auto result2 = div(10.0, 0.0)
                    .or_else([](const auto& error) {
                      return fst::result<double, std::string>("Error: " + error);
                    })
                    .map_error([](const std::string& original_error) {
                      return original_error + " (mapped)";
                    });

std::cout << "Example 2: " << result2 << '\n';

// Example 3: Chaining with `transform`
auto result3 = div(20.0, 4.0)
                    .transform([](const auto& res) {
                      return res.success() 
                          ? fst::result<std::string, std::string>(
                                fst::success_t, 
                                "Success! No error") 
                          : fst::result<std::string, std::string>(
                                fst::error_t,
                                res.error()
                                   .value_or("Unknown error"));
                    });

std::cout << "Example 3: " << result3 << '\n';

// Example 4: Using `inspect` for side effects
auto result4 = div(8.0, 2.0)
                   .inspect([](const fst::result<double, std::string>& res) {
                     if (auto value = res.success()) {
                       std::cout << "Success! Result is: " 
                                 << *value 
                                 << '\n';
                     } else {
                       std::cerr << "Error! Message: " 
                                 << res.error()
                                       .value_or("Unknown error") 
                                 << '\n';
                     }
                   });

 std::cout << "Example 4: " << result4 << '\n';

  return 0;
}

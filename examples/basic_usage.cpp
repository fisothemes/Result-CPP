#include <iostream>
#include <string>

#include "fst/result.hpp"

int main() {

  // Creating a Result with a successful integer value
  fst::result<int, std::string> res = 42;

  // Check if the Result is successful
  if (res) {
    std::cout << "Success! Value: " << res.value() << '\n';
  } else {
    std::cout << "Error: " << res.error().value() << '\n';
  }

  return 0;
}
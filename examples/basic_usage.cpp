#include "fst/result.hpp"

#include <iostream>

int main() {
    // Creating a Result with a successful integer value
    fst::result<int, std::string> res = 42;

    // Check if the Result is successful
    if (res) {
        std::cout << "Success! Value: " << res.value() << std::endl;
    } else {
        std::cout << "Error: " << res.error().value() << std::endl;
    }

    return 0;
}
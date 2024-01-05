#include <fstream>
#include <iostream>
#include <string>

#include "fst/result.hpp"

// Forward declarations
// fst::result<std::ifstream, std::string> open_file(const std::string& filename);
// fst::result<std::ofstream, std::string> write_to_file(std::ofstream& file, const std::string& content);
// fst::result<std::string, std::string> read_from_file(std::ifstream& file);
// fst::result<int, std::string> close_file(std::ifstream& file);

int main() {
  const std::string filename = "example.txt";

  // // Open file and inspect the result
  // auto file_result = open_file(filename);
  //                           // .inspect([](auto file_result) {
  //                           //     if (file_result) {
  //                           //       std::cout << "File opened successfully.\n";
  //                           //     } else {
  //                           //       std::cerr << "Error opening file: " << *file_result.error() << '\n';
  //                           //     }
  //                           // });

  // // Close file and transform the result
  // auto file = file_result.value_or();
  // auto close_result = close_file(file);

  return 0;
}

// Definitions

// Opens a file and returns a result containing either an ifstream or an error
// string
// fst::result<std::ifstream, std::string> open_file(const std::string& filename) {
//   std::ifstream file(filename);
//   return file.is_open() ? fst::result<std::ifstream, std::string>(file)
//                         : fst::result<std::ifstream, std::string>(
//                               "Failed to open file: " + filename);
// }

// // Writes content to a file and returns a result containing either the ofstream or an error string
// fst::result<std::ofstream, std::string> write_to_file(std::ofstream& file, const std::string& content) {
//     file << content;
//     return file.good() 
//         ? fst::result<std::ofstream, std::string>(std::move(file))
//         : fst::result<std::ofstream, std::string>("Failed to write to file");
// }

// // Reads content from a file and returns a result containing either the content or an error string
// fst::result<std::string, std::string> read_from_file(std::ifstream& file) {
//     std::string content;
//     std::getline(file, content);
//     return file.eof() || file.good()
//         ? fst::result<std::string, std::string>(fst::success_t, std::move(content))
//         : fst::result<std::string, std::string>(fst::error_t, "Failed to read from file");
// }

// // Closes a file and returns a result containing either success or an error string
// fst::result<int, std::string> close_file(std::ifstream& file) {
//     file.close();
//     return file.good()
//         ? fst::result<int, std::string>(0)
//         : fst::result<int, std::string>("Failed to close file");
// }
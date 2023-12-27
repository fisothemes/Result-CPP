# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Project name
project(result-cpp
    VERSION 0.1.0
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Add include directory
include_directories(include)

# Create the library
add_library(result-cpp INTERFACE)
target_include_directories(result-cpp INTERFACE include)

# Check if Doxygen is installed
find_package(Doxygen)

# If Doxygen is found, generate documentation
if(Doxygen_FOUND)
    add_custom_target(
        docs
        COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
        COMMENT "Generating documentation with Doxygen"
        VERBATIM
    )
endif()

# Add executable for Result examples
add_executable(example_basic_usage examples/basic_usage.cpp)
target_link_libraries(example_basic_usage result-cpp)

add_executable(example_chaining_operations examples/chaining_operations.cpp)
target_link_libraries(example_chaining_operations result-cpp)

add_executable(example_handling_errors examples/handling_errors.cpp)
target_link_libraries(example_handling_errors result-cpp)

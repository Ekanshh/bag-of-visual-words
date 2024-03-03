cmake_minimum_required(VERSION 3.1)
project(bow_final_project)

# Set build type if not set.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Build options
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" ON)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" ON)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# Set additional flags.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find Packages
find_package(OpenCV 4 REQUIRED)
if(OpenCV_FOUND)
  message(STATUS "Found OpenCV version ${OpenCV_VERSION}")
  message(STATUS "OpenCV directories: ${OpenCV_INCLUDE_DIRS}")
  include_directories(${OpenCV_INCLUDE_DIRS})
else()
  message(FATAL_ERROR "OpenCV not found, please read the README.md")
endif(OpenCV_FOUND)

# Code Coverage, clang-format, clan=tidy, cppcheck, Configuration:
include(TestCoverage)
include(ClangFormat)
include(StaticAnalyzers)


# Create binaries inside result directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Add subdirectories
add_subdirectory(api)
add_subdirectory(core)
add_subdirectory(interface)
add_subdirectory(src)
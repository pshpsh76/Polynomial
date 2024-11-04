cmake_minimum_required(VERSION 3.10)

project(Polynomial LANGUAGES CXX)

cmake_minimum_required(VERSION 3.13)

set(CMAKE_MODULE_PATH              "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS  ON)

find_package(Catch REQUIRED)

include(FetchContent)
include(cmake/BuildFlags.cmake)
include(cmake/TestUtils.cmake)

add_subdirectory(tests)

include_directories(include)

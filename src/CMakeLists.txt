cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_COMPILER icpx)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project("CPP Compression" VERSION 0.1)

message (${CMAKE_CXX_COMPILER})

include(FetchContent)
FetchContent_Declare(
        cli11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG        v2.3.2
)
FetchContent_MakeAvailable(cli11)

add_subdirectory(compression)
# add_subdirectory(decompression)
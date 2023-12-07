find_package(BZip2 REQUIRED)
include_directories(${BZIP2_INCLUDE_DIRS})

add_executable(serial_compression serial_compression.cpp)

# target_include_directories(
#     serial_compression
#     PRIVATE ${bzip2_SOURCE_DIR}
#     )

target_link_libraries(
    serial_compression 
    BZip2::BZip2
    CLI11::CLI11
    )

find_package(BZip2 REQUIRED)
include_directories(${BZIP2_INCLUDE_DIRS})

add_executable(serial_compression serial_compression.cpp)
add_executable(openmp_compression openmp_compression.cpp)


target_link_libraries(
    serial_compression 
    BZip2::BZip2
    CLI11::CLI11
    )

target_link_libraries(
    openmp_compression 
    BZip2::BZip2
    CLI11::CLI11
    )

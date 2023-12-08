find_package(BZip2 REQUIRED)
include_directories(${BZIP2_INCLUDE_DIRS})

add_executable(serial_decompression serial_decompression.cpp)
# add_executable(openmp_decompression openmp_decompression.cpp)


target_link_libraries(
    serial_decompression 
    BZip2::BZip2
    CLI11::CLI11
    )

# target_link_libraries(
#     openmp_decompression 
#     BZip2::BZip2
#     CLI11::CLI11
#     )

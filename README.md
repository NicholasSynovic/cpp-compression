# CPP BZip2 HPC Compression Optimization

> LUC COMP 464 HPC Final Project

## About

This project is Juan Martinez's and I's final project for our LUC COMP 464 High Performance Computing final.

The project descripton is:

```
To optimize a serial BZip2 compressor with techniques from OpenMP and MPI for compressing and decompressing files. 
Our optimizations are tested against each other, as well as other common applications for compressing and decompressing files.
```

## Dependencies

The following software is required to run/ compile our code

- git
- cmake
- bzip2
- tar
- lbzip2
- wget
- unzip
- time

## How to compile

We use `cmake` to build our project.

Within the `src` directory is a `build.bash` script that can be ran to automatically build the software.

## How to run benchmarks

To run our benchmarks, please follow this order:

1. Run `downloadTestData.bash` to download and unzip the required data
2. Run `compressionBenchmark.bash` to evaluate compression optimizations
3. Run `decompressionBenchmark.bash` to evaluate decompression optimizations

Benchmark results are stored in `benchmarks/{de}compression` as CSV files for each tested application.

Benchmark results are in seconds.

CSV files column headers are ordered as: `System,Process,Real Time`

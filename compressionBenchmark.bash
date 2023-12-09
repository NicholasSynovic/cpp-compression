#!/bin/bash

# Benchmarking script
# Each program is ran 100 times and times are saved to 
# benchmarks/$PROGRAM_NAME.csv

# To keep everything fair, the maximum compression ratio is choosen for each 
# compressor

TIME_FORMAT="%S,%U,%e"

mkdir -p benchmarks/compression

# Tar Compression (Serial)
for i in $(seq 1 10);
do
    echo "Tar Compression: $i / 10"
    /usr/bin/time --append --output benchmarks/compression/tar.csv --format=$TIME_FORMAT --quiet tar -cjf testData/enwik8.bz2 testData/enwik8
    rm testData/enwik8.bz2
done

# BZip2 Compression (Serial)
for i in $(seq 1 10);
do
    echo "BZip 2 Compression: $i / 10"
    /usr/bin/time --append --output benchmarks/compression/bzip2.csv --format=$TIME_FORMAT --quiet bzip2 --compress --keep --force --quiet --best testData/enwik8 testData/enwik8.bz2 
    rm testData/enwik8.bz2
done

# Serial Compression (Serial)
for i in $(seq 1 10);
do
    echo "Serial Compression: $i / 10"
    /usr/bin/time --append --output benchmarks/compression/serial_compression.csv --format=$TIME_FORMAT --quiet src/bin/serial_compression -i testData/enwik8 -o testData/enwik8.bz2 
    rm testData/enwik8.bz2
done

# LBZip2 Compression (Parallel)
for i in $(seq 1 10);
do
    echo "LBZip2 Compression: $i / 10"
    /usr/bin/time --append --output benchmarks/compression/lbzip2.csv --format=$TIME_FORMAT --quiet lbzip2 --keep --compress --best --force testData/enwik8
    rm testData/enwik8.bz2
done

# OpenMP Compression (Parallel)
for i in $(seq 1 10);
do
    echo "OpenMP Compression: $i / 10"
    /usr/bin/time --append --output benchmarks/compression/openmp_compression.csv --format=$TIME_FORMAT --quiet src/bin/openmp_compression -i testData/enwik8 -o testData/enwik8.bz2 
    rm testData/enwik8.bz2
done

#!/bin/bash

# Benchmarking script
# Each program is ran $MAX_ITER times and times are saved to 
# benchmarks/$PROGRAM_NAME.csv

# To keep everything fair, the maximum compression ratio is choosen for each 
# compressor

# CHANGE THESE TO ADJUST BENCHMARK FUNCTIONALITY
MAX_ITER=100
INPUT_FILEPATH="testData/enwik8"
OUTPUT_BENCHMARK_DIR="benchmarks/compression"
# ====

ACTION_WORD="Compression"
TIME_FORMAT="%S,%U,%e"

BACKUP_INPUT_FILEPATH="$INPUT_FILEPATH.bk"
BZIP2_ARCHIVE_FILEPATH="$INPUT_FILEPATH.bz2"

# Create $OUTPUT_BENCHMARK_DIR
mkdir -p $OUTPUT_BENCHMARK_DIR

# Delete $BZIP2_ARCHIVE_FILEPATH
# Delete $INPUT_FILEPATH
rm --force $BZIP2_ARCHIVE_FILEPATH $INPUT_FILEPATH

# Copy $BACKUP_INPUT_FILEPATH to $INPUT_FILEPATH
cp $BACKUP_INPUT_FILEPATH $INPUT_FILEPATH

# Run benchmarks

# Example benchmark
# for i in range(1 - $MAX_ITER)
#   echo "PROGRAM_NAME $ACTION_WORD: $i / $MAX_ITER
#   cp $BACKUP_INPUT_FILEPATH $INPUT_FILEPATH
#   /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/PROGRAM_NAME.csv --format=$TIME_FORMAT --quiet PROGRAM_NAME PROGRAM_OPTIONS
#   rm $BZIP2_ARCHIVE_FILEPATH
# done

# Tar Compression (Serial)
# for i in $(seq 1 $MAX_ITER);
# do
#     echo "Tar $ACTION_WORD: $i / $MAX_ITER"
#     cp $BACKUP_INPUT_FILEPATH $INPUT_FILEPATH
#     /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/tar.csv --format=$TIME_FORMAT --quiet tar -cjf $BZIP2_ARCHIVE_FILEPATH $INPUT_FILEPATH
#     rm $BZIP2_ARCHIVE_FILEPATH
# done

# BZip2 Compression (Serial)
for i in $(seq 1 $MAX_ITER);
do
    echo "BZip2 $ACTION_WORD: $i / $MAX_ITER"
    cp $BACKUP_INPUT_FILEPATH $INPUT_FILEPATH
    /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/bzip2.csv --format=$TIME_FORMAT --quiet bzip2 --compress --keep --force --quiet --best $INPUT_FILEPATH $BZIP2_ARCHIVE_FILEPATH
    rm $BZIP2_ARCHIVE_FILEPATH
done

# Serial Compression (Serial)
for i in $(seq 1 $MAX_ITER);
do
    echo "Serial $ACTION_WORD: $i / $MAX_ITER"
    cp $BACKUP_INPUT_FILEPATH $INPUT_FILEPATH
    /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/serial_compression.csv --format=$TIME_FORMAT --quiet src/bin/serial_compression -i $INPUT_FILEPATH -o $BZIP2_ARCHIVE_FILEPATH
    rm $BZIP2_ARCHIVE_FILEPATH
done

# LBZip2 Compression (Parallel)
for i in $(seq 1 $MAX_ITER);
do
    echo "LBZip2 $ACTION_WORD: $i / $MAX_ITER"
    cp $BACKUP_INPUT_FILEPATH $INPUT_FILEPATH
    /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/lbzip2.csv --format=$TIME_FORMAT --quiet lbzip2 --compress --keep --force --quiet --best $INPUT_FILEPATH
    rm $BZIP2_ARCHIVE_FILEPATH
done

# OpenMP Compression (Parallel)
for i in $(seq 1 $MAX_ITER);
do
    echo "OpenMP $ACTION_WORD: $i / $MAX_ITER"
    cp $BACKUP_INPUT_FILEPATH $INPUT_FILEPATH
    /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/openmp_compression.csv --format=$TIME_FORMAT --quiet src/bin/openmp_compression -i $INPUT_FILEPATH -o $BZIP2_ARCHIVE_FILEPATH
    rm $BZIP2_ARCHIVE_FILEPATH
done

# Delete $BZIP2_ARCHIVE_FILEPATH
# Delete $INPUT_FILEPATH
rm --force $BZIP2_ARCHIVE_FILEPATH $INPUT_FILEPATH

# Copy $BACKUP_INPUT_FILE to $INPUT_FILEPATH
cp $BACKUP_INPUT_FILEPATH $INPUT_FILEPATH

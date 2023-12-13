#!/bin/bash

# Benchmarking script
# Each program is ran $MAX_ITER times and times are saved to 
# benchmarks/$PROGRAM_NAME.csv

# To keep everything fair, the maximum compression ratio is choosen for each 
# compressor

# CHANGE THESE TO ADJUST BENCHMARK FUNCTIONALITY
MAX_ITER=100
BZIP2_ARCHIVE_FILEPATH="testData/enwik9.bz2"
OUTPUT_BENCHMARK_DIR="benchmarks/decompression"
OUTPUT_FILEPATH="testData/enwik9"
# ====

ACTION_WORD="Decompression"
TIME_FORMAT="%S,%U,%e,%K,%I,%O"

BACKUP_OUTPUT_FILEPATH="$OUTPUT_FILEPATH.bk"
BZIP2_BACKUP_ARCHIVE_FILEPATH="$BZIP2_ARCHIVE_FILEPATH.bk"

# Create $OUTPUT_BENCHMARK_DIR
mkdir -p $OUTPUT_BENCHMARK_DIR

# Delete $BZIP2_ARCHIVE_FILEPATH
# Delete $BZIP2_BACKUP_ARCHIVE_FILEPATH
rm --force $BZIP2_ARCHIVE_FILEPATH $BZIP2_BACKUP_ARCHIVE_FILEPATH

# Copy $BACKUP_OUTPUT_FILEPATH to $OUTPUT_FILEPATH
cp $BACKUP_OUTPUT_FILEPATH $OUTPUT_FILEPATH

# Create $BZIP2_ARCHIVE_FILEPATH
src/bin/serial_compression -i $OUTPUT_FILEPATH -o $BZIP2_ARCHIVE_FILEPATH

# Create $BZIP2_BACKUP_ARCHIVE_FILEPATH
cp $BZIP2_ARCHIVE_FILEPATH $BZIP2_BACKUP_ARCHIVE_FILEPATH

# Delete $OUTPUT_FILEPATH
rm --force $OUTPUT_FILEPATH

# Run benchmarks

# Example benchmark
# for i in range(1 - $MAX_ITER)
#   echo "PROGRAM_NAME $ACTION_WORD: $i / $MAX_ITER
#   cp $BZIP2_BACKUP_ARCHIVE_FILEPATH $BZIP2_ARCHIVE_FILEPATH
#   /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/PROGRAM_NAME.csv --format=$TIME_FORMAT --quiet PROGRAM_NAME PROGRAM_OPTIONS
#   rm $OUTPUT_FILEPATH

# Tar Decompression (Serial)
# NOTE: Tar does not decompress .bz2 archives, needs .tar.bz2 archives
# for i in $(seq 1 $MAX_ITER);
# do
#     echo "Tar $ACTION_WORD: $i / $MAX_ITER"
#     cp $BZIP2_BACKUP_ARCHIVE_FILEPATH $BZIP2_ARCHIVE_FILEPATH
#     /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/tar.csv --format=$TIME_FORMAT --quiet tar -jxf $BZIP2_ARCHIVE_FILEPATH -C "testData"
#     rm $OUTPUT_FILEPATH
# done

# BZip2 Decompression (Serial)
for i in $(seq 1 $MAX_ITER);
do
    echo "BZip2 $ACTION_WORD: $i / $MAX_ITER"
    cp $BZIP2_BACKUP_ARCHIVE_FILEPATH $BZIP2_ARCHIVE_FILEPATH
    /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/bzip2.csv --format=$TIME_FORMAT --quiet bzip2 --decompress --keep --force --quiet $BZIP2_ARCHIVE_FILEPATH
    rm $OUTPUT_FILEPATH
done

# Serial Decompression (Serial)
for i in $(seq 1 $MAX_ITER);
do
    echo "Serial $ACTION_WORD: $i / $MAX_ITER"
    cp $BZIP2_BACKUP_ARCHIVE_FILEPATH $BZIP2_ARCHIVE_FILEPATH
    /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/serial_compression.csv --format=$TIME_FORMAT --quiet src/bin/serial_decompression -i $BZIP2_ARCHIVE_FILEPATH -o $OUTPUT_FILEPATH
    rm $OUTPUT_FILEPATH
done

# LBZip2 Decompression (Parallel)
for i in $(seq 1 $MAX_ITER);
do
    echo "LBZip2 $ACTION_WORD: $i / $MAX_ITER"
    cp $BZIP2_BACKUP_ARCHIVE_FILEPATH $BZIP2_ARCHIVE_FILEPATH
    /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/lbzip2.csv --format=$TIME_FORMAT --quiet lbzip2 --keep --decompress --force --quiet $BZIP2_ARCHIVE_FILEPATH
    rm $OUTPUT_FILEPATH
done

# OpenMP Decompression (Parallel)
openmpDecompression ()  {
    for i in $(seq 1 $MAX_ITER);
    do
        echo "OpenMP $ACTION_WORD: $i / $MAX_ITER"
        cp $BZIP2_BACKUP_ARCHIVE_FILEPATH $BZIP2_ARCHIVE_FILEPATH
        /usr/bin/time --append --output $OUTPUT_BENCHMARK_DIR/openmp_compression_$1.csv --format=$TIME_FORMAT --quiet src/bin/openmp_decompression -i $BZIP2_ARCHIVE_FILEPATH -o $OUTPUT_FILEPATH
        rm $OUTPUT_FILEPATH
    done
}

OMP_NUM_THREADS=1
openmpDecompression $OMP_NUM_THREADS

OMP_NUM_THREADS=2
openmpDecompression $OMP_NUM_THREADS

OMP_NUM_THREADS=4
openmpDecompression $OMP_NUM_THREADS

OMP_NUM_THREADS=8
openmpDecompression $OMP_NUM_THREADS

OMP_NUM_THREADS=16
openmpDecompression $OMP_NUM_THREADS


# Delete $BZIP2_ARCHIVE_FILEPATH
# Delete $BZIP2_BACKUP_ARCHIVE_FILEPATH
# Delete $OUTPUT_FILEPATH
rm --force $BZIP2_ARCHIVE_FILEPATH $BZIP2_BACKUP_ARCHIVE_FILEPATH $OUTPUT_FILEPATH

# Copy $BACKUP_OUTPUT_FILEPATH to $OUTPUT_FILEPATH
cp $BACKUP_OUTPUT_FILEPATH $OUTPUT_FILEPATH

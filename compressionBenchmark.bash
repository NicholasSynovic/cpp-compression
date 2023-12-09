#!/bin/bash

# Benchmarking script
# Each program is ran 100 times and times are saved to 
# benchmarks/$PROGRAM_NAME.csv

# To keep everything fair, the maximum compression ratio is choosen for each 
# compressor

TIME_FORMAT="%S,%U,%e"
OUTPUT_DIR="benchmarks/compression"
WORD="Compression"
INPUT_FILE="testData/enwik8"
OUTPUT_FILE="testData/enwik8.bz2"
BACKUP_FILE="testData/enwik8.bk"

mkdir -p $OUTPUT_DIR

# Tar Compression (Serial)
for i in $(seq 1 10);
do
    echo "Tar $WORD: $i / 10"
    # Rest benchmarking file for safety
    cp $BACKUP_FILE $INPUT_FILE
    /usr/bin/time --append --output $OUTPUT_DIR/tar.csv --format=$TIME_FORMAT --quiet tar -cjf $OUTPUT_FILE $INPUT_FILE
    rm $OUTPUT_FILE
done

# BZip2 Compression (Serial)
for i in $(seq 1 10);
do
    echo "BZip 2 $WORD: $i / 10"
    # Rest benchmarking file for safety
    cp $BACKUP_FILE $INPUT_FILE
    /usr/bin/time --append --output $OUTPUT_DIR/bzip2.csv --format=$TIME_FORMAT --quiet bzip2 --compress --keep --force --quiet --best $INPUT_FILE $OUTPUT_FILE
    rm $OUTPUT_FILE
done

# Serial Compression (Serial)
for i in $(seq 1 10);
do
    echo "Serial $WORD: $i / 10"
    # Rest benchmarking file for safety
    cp $BACKUP_FILE $INPUT_FILE
    /usr/bin/time --append --output $OUTPUT_DIR/serial_compression.csv --format=$TIME_FORMAT --quiet src/bin/serial_compression -i $INPUT_FILE -o $OUTPUT_FILE
    rm $OUTPUT_FILE
done

# LBZip2 Compression (Parallel)
for i in $(seq 1 10);
do
    echo "LBZip2 $WORD: $i / 10"
    # Rest benchmarking file for safety
    cp $BACKUP_FILE $INPUT_FILE
    /usr/bin/time --append --output $OUTPUT_DIR/lbzip2.csv --format=$TIME_FORMAT --quiet lbzip2 --keep --compress --best --force $INPUT_FILE
    rm $OUTPUT_FILE
done

# OpenMP Compression (Parallel)
for i in $(seq 1 10);
do
    echo "OpenMP $WORD: $i / 10"
    # Rest benchmarking file for safety
    cp $BACKUP_FILE $INPUT_FILE
    /usr/bin/time --append --output $OUTPUT_DIR/openmp_compression.csv --format=$TIME_FORMAT --quiet src/bin/openmp_compression -i $INPUT_FILE -o $OUTPUT_FILE
    rm $OUTPUT_FILE
done

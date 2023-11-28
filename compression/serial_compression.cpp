#include <iostream>
#include <fstream>
#include <bzlib.h>

std::ifstream inputFile("path/to/input/file", std::ios_base::binary);
std::ofstream outputFile("path/to/output/file.bz2", std::ios_base::binary);

bz_stream strm = {0};
int blockSize100k = 9; // Compression level (1-9)
int verbosity = 0;
int workFactor = 30;

if (BZ2_bzCompressInit(&strm, blockSize100k, verbosity, workFactor) != BZ_OK) {
    std::cerr << "Failed to initialize compression." << std::endl;
    // Handle error
}

const int bufferSize = 1000000;
char* inBuffer = new char[bufferSize];
char* outBuffer = new char[bufferSize];

int action = BZ_RUN;

while (!inputFile.eof()) {
    inputFile.read(inBuffer, bufferSize);
    strm.avail_in = inputFile.gcount();
    strm.next_in = inBuffer;

    do {
        strm.avail_out = bufferSize;
        strm.next_out = outBuffer;
        int ret = BZ2_bzCompress(&strm, action);
        if (ret != BZ_RUN_OK && ret != BZ_FINISH_OK) {
            std::cerr << "BZip2 compression error." << std::endl;
            // Handle error
        }

        int have = bufferSize - strm.avail_out;
        outputFile.write(outBuffer, have);
    } while (strm.avail_out == 0);

    if (inputFile.eof()) {
        action = BZ_FINISH;
    }
}

delete[] inBuffer;
delete[] outBuffer;

BZ2_bzCompressEnd(&strm);
inputFile.close();
outputFile.close();


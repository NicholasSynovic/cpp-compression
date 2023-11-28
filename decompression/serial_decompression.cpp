#include <iostream>
#include <fstream>
#include <bzlib.h>

std::ifstream inputFile("path/to/compressed/file.bz2", std::ios_base::binary);
std::ofstream outputFile("path/to/decompressed/file", std::ios_base::binary);

bz_stream strm = {0};
if (BZ2_bzDecompressInit(&strm, 0, 0) != BZ_OK) {
    std::cerr << "Failed to initialize decompression." << std::endl;
    // Handle error
}

const int bufferSize = 1000000;
char* inBuffer = new char[bufferSize];
char* outBuffer = new char[bufferSize];

while (!inputFile.eof()) {
    inputFile.read(inBuffer, bufferSize);
    strm.avail_in = inputFile.gcount();
    strm.next_in = inBuffer;

    do {
        strm.avail_out = bufferSize;
        strm.next_out = outBuffer;
        int ret = BZ2_bzDecompress(&strm);
        if (ret != BZ_OK && ret != BZ_STREAM_END) {
            std::cerr << "BZip2 decompression error." << std::endl;
            // Handle error
        }

        int have = bufferSize - strm.avail_out;
        outputFile.write(outBuffer, have);
    } while (strm.avail_out == 0 && ret != BZ_STREAM_END);

    if (inputFile.eof() && ret == BZ_STREAM_END) {
        break;
    }
}

delete[] inBuffer;
delete[] outBuffer;

BZ2_bzDecompressEnd(&strm);
inputFile.close();
outputFile.close();

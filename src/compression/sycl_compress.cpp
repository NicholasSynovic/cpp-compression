#include <array>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>
#include <vector>

#include <CLI/CLI.hpp>
#include <bzlib.h>

static int BZIP_COMPRESSION_LEVEL = 9;
static int BZIP_WORK_FACTOR = 0;
static int BZIP_VERBOSE = 0;

std::vector<char> compressChunk(const std::vector<char> &datum) {
    using uint = unsigned int;

    float magicNumber = 1.02; // What does this do?
    int sizeBuffer = 600;     // Buffer to ensure that there is enough space in
                              // the compressed buffer

    const uint datumSize = datum.size();

    uint compressedDatumSize = uint(datumSize * magicNumber) + sizeBuffer;

    std::vector<char> compressedDatum(compressedDatumSize);

    char *datumPtr = const_cast<char *>(datum.data());
    char *compressedDatumPtr = const_cast<char *>(compressedDatum.data());

    // Actual function to compress chunks
    auto compressionError = BZ2_bzBuffToBuffCompress(
        compressedDatumPtr, &compressedDatumSize, datumPtr, datumSize,
        BZIP_COMPRESSION_LEVEL, BZIP_VERBOSE, BZIP_WORK_FACTOR);

    // Error checking to see if the function actual compressed the chunk
    if (compressionError != BZ_OK) {
        fprintf(stderr, "bzBuffToBuffCompress failed with %d\n",
                compressionError);
        exit(1);
    }

    // Reclaim any extra memory.
    compressedDatum.resize(compressedDatumSize);

    // Rather than copying the the output of the function, move it
    return std::move(compressedDatum);
}

std::array<size_t, 3> compressStream(FILE *inputStream, FILE *outputStream) {
    // How big are the input chunks?
    // The compression level switch is [1-9] which means 100-900k.
    size_t compressedChunkSize = BZIP_COMPRESSION_LEVEL * 100 * 1024;

    int numberOfChunks = 0;
    size_t inputBytes = 0;
    size_t outputBytes = 0;

    // While not at end of file marker
    while (not(feof(inputStream))) {
        // Space for the input stream data.
        std::vector<char> chunk(compressedChunkSize);

        // Read a chunk of data from the input stream.
        auto dataBytes =
            fread(chunk.data(), sizeof(char), compressedChunkSize, inputStream);

        inputBytes += dataBytes;

        // Resize the bytes read to reduce
        if (dataBytes < compressedChunkSize)
            chunk.resize(dataBytes);

        auto compressedChunk = compressChunk(chunk);

        outputBytes += compressedChunk.size();

        fwrite(compressedChunk.data(), sizeof(char), compressedChunk.size(),
               outputStream);

        numberOfChunks++;
    }

    return {size_t(numberOfChunks), inputBytes, outputBytes};
}

int main(int argc, char **argv) {
    std::string inputFP;
    std::string outputFP;

    CLI::App app{"Serial BZIP Compression"};
    app.option_defaults()->always_capture_default(true);

    app.add_option("-i,--input-filepath", inputFP,
                   "File to compress with BZIP2")
        ->required()
        ->check(CLI::ExistingFile);

    app.add_option("-o,--output-filepath", outputFP, "Output filepath")
        ->required()
        ->check(CLI::NonexistentPath);

    CLI11_PARSE(app, argc, argv);

    FILE *inputFPPtr = fopen(inputFP.c_str(), "rb");
    if (inputFPPtr == nullptr) {
        fprintf(stderr, "Error opening input file %s\n", inputFP.c_str());
        return 1;
    }

    FILE *outputFPPtr = fopen(outputFP.c_str(), "wb");
    if (outputFPPtr == nullptr) {
        fprintf(stderr, "Error: failed to open %s for writing\n",
                outputFP.c_str());
        return 2;
    }

    std::array<size_t, 3> result;
    result = compressStream(inputFPPtr, outputFPPtr);

    fclose(inputFPPtr);
    fclose(outputFPPtr);

    return 0;
}

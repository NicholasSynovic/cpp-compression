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
    size_t chunk_size = BZIP_COMPRESSION_LEVEL * 100 * 1024;
    int K = 16; // number of blocks to read, compress, and write at a time

    int n_chunks = 0;
    size_t ibytes = 0;
    size_t obytes = 0;

    while ( not(feof(inputStream)) )
    {
        std::vector<std::vector<char>> chunks(K, std::vector<char>(chunk_size));
        std::vector<size_t> bytes_read(K, 0);
        std::vector<std::vector<char>> odata(K);

        // read K blocks of data from the input stream
        for (int i = 0; i < K && !feof(inputStream); ++i)
        {
    
            bytes_read[i] = fread(chunks[i].data(), sizeof(char), chunk_size, inputStream);
            ibytes += bytes_read[i];

            if (bytes_read[i] < chunk_size)
                chunks[i].resize(bytes_read[i]);
        }

        // in parallel, compress the K blocks
        #pragma omp parallel for
        for (int i = 0; i < K; ++i)
        {
            if (bytes_read[i] > 0)
                odata[i] = compressChunk(chunks[i]);
        }

        // write the K compressed blocks to the output stream
        for (int i = 0; i < K; ++i)
        {
            if (bytes_read[i] > 0) {
                obytes += odata[i].size();
                fwrite(odata[i].data(), sizeof(char), odata[i].size(), outputStream);
            }
            n_chunks ++;
        }
    }
    return {size_t(n_chunks), ibytes, obytes};
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

#include <sstream>
#include <sys/stat.h>
#include <vector>

#include <CLI/CLI.hpp>
#include <bzlib.h>

static int BZIP_COMPRESSION_LEVEL = 9;
static int BZIP_WORK_FACTOR = 0;
static int BZIP_VERBOSE = 0;


std::vector<char> compressChunk(const std::vector<char> &datum) {
    using uint = unsigned int;
    const uint datumSize = datum.size();

    uint compressedDatumSize = uint(datumSize * 1.02) + 600;

    std::vector<char> compressedDatum(compressedDatumSize);

    char *datumPtr = const_cast<char *>(datum.data());
    char *compressedDatumPtr = const_cast<char *>(compressedDatum.data());

    auto ierr = BZ2_bzBuffToBuffCompress(compressedDatumPtr, &compressedDatumSize, datumPtr, datumSize,
                                         BZIP_COMPRESSION_LEVEL, BZIP_VERBOSE,
                                         BZIP_WORK_FACTOR);

    if (ierr != BZ_OK) {
        fprintf(stderr, "bzBuffToBuffCompress failed with %d\n", ierr);
        exit(1);
    }

    // Reclaim any extra memory.
    compressedDatum.resize(compressedDatumSize);

    return std::move(compressedDatum);
}

int main(int argc, char **argv) {
    std::string inputFP;
    std::string outputFP;

    CLI::App app{"Serial BZIP Compression"};
    app.option_defaults()->always_capture_default(true);

    app.add_option("-f,--filepath", inputFP, "File to compress with BZIP2")
        ->check(CLI::ExistingFile);

    app.add_option("-o,--output-filepath", outputFP, "Output filepath")
        ->check(CLI::NonexistentPath);

    CLI11_PARSE(app, argc, argv);

    return 0;
}

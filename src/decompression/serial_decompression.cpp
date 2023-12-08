#include <array>
#include <sstream>

#include <CLI/CLI.hpp>
#include <bzlib.h>

static const char BZIP_HEADER[] = "BZh91AY&SY";
static const unsigned char BZIP_HEADER_0[] = {
    'B', 'Z', 'h', '9', 0x17, 0x72, 0x45, 0x38, 0x50, 0x90, 0};
static int BZIP_COMPRESSION_LEVEL = 9;
static int BZIP_WORK_FACTOR = 0;
static int BZIP_VERBOSE = 0;

std::array<size_t, 3> decompressStream(FILE *inputStream, FILE *outputStream) {
    std::string streamIn;
    std::string bz2header = BZIP_HEADER;
    size_t ibytes = 0;
    size_t obytes = 0;
    int nblocks = 0;
    int blockSize100k = 0;
    bool isFirstBlock = true;

    // Ensure that there is enough space to store the deocmpressed buffer
    const size_t decompressedBufferSize =
        (BZIP_COMPRESSION_LEVEL * 1024 * 100 + 600);

    std::string _bufIn(decompressedBufferSize, ' ');
    char *bufIn = const_cast<char *>(_bufIn.data());

    while (true) {

        // Read in a chunk of data for scanning.
        size_t nread = fread(bufIn, sizeof(char), bufSize, istream);

        // Test if we've hit the EOF or some other error.
        bool isEnd = feof(istream);
        if (nread < bufSize)
            if (not(isEnd)) {
                fprintf(stderr, "Error reading file %d\n", __LINE__);
                exit(1);
            }

        // Concatenate the existing stream (anything left over) with the new
        // chunk.
        streamIn.append(bufIn, bufIn + nread);
#ifdef _DEBUG
        fprintf(stderr, "nread = %lu, isEnd=%d length=%lu\n", nread, isEnd,
                streamIn.length());
#endif

        // Special code if this is the precise head of a block.
        if (isFirstBlock) {
            isFirstBlock = false;

            if (nread < bz2header.length()) {
                fprintf(stderr, "File not a bz2 file\n");
                exit(1);
            }

            size_t pos = streamIn.find(bz2header.c_str(), 0, 3);
            if (pos != 0) {
                fprintf(stderr, "File not a bz2 file 2\n");
                exit(1);
            }

            // Deduce the block size used and assume all blocks are the same.
            blockSize100k = atoi(streamIn.substr(3, 1).c_str());
            bz2header[3] = streamIn[3];
        }

        size_t pos;

        // Sanity check that the stream starts with a full header.
        {
            pos = streamIn.find(bz2header, 0);
            if (pos != 0) {
                fprintf(stderr, "bz2header not valid %d %s\n", __LINE__,
                        streamIn.substr(0, bz2header.length()).c_str());
                exit(1);
            }
        }

        int numBlocksRead = 0;
        std::vector<std::vector<char>> idata;
        std::vector<uint32_t> isize;

        // Scan the loaded data stream and split into bz blocks. There will
        // likely be a bit left over.
        size_t next;
        while ((next = streamIn.find(bz2header, pos + bz2header.length())) !=
                   std::string::npos or
               isEnd) {
            if (next == std::string::npos and isEnd)
                next = streamIn.length();

            int i = numBlocksRead++;
            size_t size = next - pos;
            auto begin = streamIn.data() + pos;
            std::vector<char> block(streamIn.data() + pos,
                                    streamIn.data() + next);
            isize.push_back(size);
            idata.push_back(block);

            pos = next;
            if (next == streamIn.length() and isEnd)
                break;
        }

        // Append the remaining segment into the stream buffer.
        streamIn.erase(0, pos);

        ibytes += nread;

        std::vector<std::vector<char>> odata(numBlocksRead);
        std::vector<uint32_t> osize(numBlocksRead, 0);
        std::vector<int> err(numBlocksRead, 0);

        // Decompress the loaded blocks.
        for (int i = 0; i < numBlocksRead; i++) {
            osize[i] = blockSize100k * 102400;
            odata[i].resize(osize[i]);

            int ierr = BZ2_bzBuffToBuffDecompress(odata[i].data(), &osize[i],
                                                  idata[i].data(), isize[i], 0,
                                                  bz_verbose);

            err[i] = ierr;
        }

        // Write out the decompressed streams.
        for (int i = 0; i < numBlocksRead; i++) {
            if (isize[i] > 0)
                if (err[i] == BZ_OK) {
                    fwrite(odata[i].data(), sizeof(char), osize[i], ostream);
                    obytes += osize[i];

                    osize[i] = 0;
                } else {
                    fprintf(stderr, "Error decompressing block %d %d\n", i,
                            err[i]);
                    exit(1);
                }
        }

        nblocks += numBlocksRead;

        if (isEnd)
            break;
    }

#ifdef _DEBUG
    fprintf(stderr, "Decompress: BytesIn=%lu, BytesOut=%lu, %d\n", ibytes,
            obytes, nblocks);
#endif

    return {size_t(nblocks), ibytes, obytes};
}

int main(int argc, char **argv) {
    std::string inputFP;

    CLI::App app{"Serial BZIP Compression"};
    app.option_defaults()->always_capture_default(true);

    app.add_option("-i,--input-filepath", inputFP,
                   "File to compress with BZIP2")
        ->required()
        ->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    FILE *inputFPPtr = fopen(inputFP.c_str(), "rb");
    if (inputFPPtr == nullptr) {
        fprintf(stderr, "Error opening input file %s\n", inputFP.c_str());
        return 1;
    }

    return 0;
}

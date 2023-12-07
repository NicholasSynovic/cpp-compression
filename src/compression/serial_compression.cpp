#include <sstream>
#include <sys/stat.h>

#include <CLI/CLI.hpp>
#include <bzlib.h>

int main(int argc, char **argv)  {
    std::string inputFP;
    std::string outputFP;

    CLI::App app{"Serial BZIP Compression"};
    app.option_defaults()->always_capture_default(true);

    app.add_option("-f,--filepath", inputFP,
                   "File to compress with BZIP2")
       ->check(CLI::ExistingFile);

    app.add_option("-o,--output-filepath", outputFP,
                   "Output filepath")
       ->check(CLI::NonexistentPath);

    CLI11_PARSE(app, argc, argv);

    return 0;
}
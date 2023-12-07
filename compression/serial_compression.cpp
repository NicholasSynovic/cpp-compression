#include <iostream>
#include <CLI/CLI.hpp>

int main(int argc, char **argv)  {
    int numberOfBits = 0;

    CLI::App app{"Bloom Filter Implementation"};
    app.option_defaults()->always_capture_default(true);

    app.add_option("-b,--number-of-bits", numberOfBits,
                 "Number of bits to allocate to the bit vector")
      ->check(CLI::PositiveNumber.description(" >0"));
    
    CLI11_PARSE(app, argc, argv);
    
    return 0;
}


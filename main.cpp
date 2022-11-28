#include "parser.h"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    char const* filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in) {
        std::cerr << "Error: Could not open input file: "
                  << filename << std::endl;
        return 1;
    }

    std::string source_code; // Read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping
    std::copy(
            std::istream_iterator<char>(in),
            std::istream_iterator<char>(),
            std::back_inserter(source_code));

    auto iter = source_code.begin();
    auto end = source_code.end();

    parasl::Parser parser{iter, end};

    if (parser.Run()) {
        std::cout << "Parsing succeeded" << "\n";
        return 0;
    } else {
        std::cerr << "Parsing failed\n";
        return 1;
    }
}

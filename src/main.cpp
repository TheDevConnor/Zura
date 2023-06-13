#include <iostream>
#include <fstream>
#include <sstream>

#include "parser/lexer/lexer.h"
#include "lexer_stress_test.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file> [stress]" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Could not open file " << filename << std::endl;
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    init_tokenizer(source.c_str());

    if (argc > 2 && std::string(argv[2]) == "stress") {
        std::cout << "Running lexer stress test..." << std::endl;
        runStressTest(generateStressTestInput(200000));
    } else {
        Token token;
        do {
            token = scan_token();
            if (token.kind < 10) std::cout << " " << token.kind << " | " << std::string(token.start, token.length) << std::endl;
            else std::cout << token.kind << " | " << std::string(token.start, token.length) << std::endl;
        } while (token.kind != EOF_TOKEN);
    }

    return 0;
}

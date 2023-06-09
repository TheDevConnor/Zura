#include <iostream>
#include <fstream>
#include "lexer/lexer.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    if(!file) {
        std::cerr << "Could not open file " << filename << std::endl;
        return 1;
    }

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    init_tokenizer(source.c_str());

    Token token;
    do {
        token = scan_token();
        printf("%d %d %.*s \n", token.line, token.column, token.length, token.start);
    } while (token.kind != EOF_TOKEN);

    return 0;
}
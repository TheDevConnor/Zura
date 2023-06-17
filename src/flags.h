#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

void repl() {
    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line == "exit") {
            break;
        }
        std::cout << line << std::endl;
    }
}

void flags(int argc, char* argv[]) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        std::cout << "v0.0.1" << std::endl;
        exit(0);
    }
    
    if (argc == 2 && strcmp(argv[1], "--repl") == 0) {
        repl();
        exit(0);
    }

    // Check if we have a --help flag
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --repl\t\t\tStarts a REPL" << std::endl;
        std::cout << "  --version\t\t\tPrints the version of the compiler" << std::endl;
        std::cout << "  --help\t\t\tPrints this help message" << std::endl;
        exit(0);
    }
}
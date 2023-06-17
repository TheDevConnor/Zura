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
        interpret(line.c_str());
    }
}

static char* read_file(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open file \"" << path << "\"." << std::endl;
        exit(74);
    }

    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    char* buffer = new char[file_size + 1];
    file.read(buffer, file_size);
    file.close();

    buffer[file_size] = '\0';
    return buffer;
}

void run_file(const char* path) {
    const char* source = read_file(path);

    InterpretResult result = interpret(source);

    if (result == InterpretResult::INTERPRET_COMPILE_ERROR) exit(65);
    else if (result == InterpretResult::INTERPRET_COMPILE_ERROR) exit(70);
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
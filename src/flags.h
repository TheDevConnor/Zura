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
    FILE* file = fopen(path, "rb");

    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);

    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);

    if (bytes_read < file_size) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

static void run_file(const char* path) {
    char* source = read_file(path);
    InterpretResult result = interpret(source);
    free(source);

    if (result == INTERPRET_COMPILE_ERROR) return exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) return exit(70);
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
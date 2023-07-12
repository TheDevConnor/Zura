#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using namespace std;

static char* read_file(const char* path) {
    ifstream file(path, ios::binary);
    if (!file) {
        cerr << "Could not open file \"" << path << "\"." << endl;
        exit(74);
    }

    file.seekg(0, ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, ios::beg);

    char* buffer = new char[file_size + 1];
    file.read(buffer, file_size);
    file.close();

    buffer[file_size] = '\0';
    return buffer;
}

void run_file(const char* path) {
    const char* source = read_file(path);
    // Check to make sure that we have  a .zu file extension
    if (strcmp(path + strlen(path) - 3, ".zu") != 0) {
        cerr << "File \"" << path << "\" does not have a .zu extension." << endl;
        exit(65);
    }

    InterpretResult result = interpret(source);

    if (result == InterpretResult::INTERPRET_COMPILE_ERROR) exit(65);
    else if (result == InterpretResult::INTERPRET_COMPILE_ERROR) exit(70);
}

void flags(int argc, char* argv[]) {
    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        cout << "v0.0.1" << endl;
        exit(0);
    }
    
    if (argc == 2 && strcmp(argv[1], "--repl") == 0) {
        char line[1024];
        for(;;) {
            cout << "> ";
            if (!fgets(line, sizeof(line), stdin)) break;
            interpret(line);
        }
    }

    // Check if we have a --help flag
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        cout << "Usage: " << argv[0] << " [options]" << endl;
        cout << "Options:" << endl;
        cout << "  --repl\t\t\tStarts a REPL" << endl;
        cout << "  --version\t\t\tPrints the version of the compiler" << endl;
        cout << "  --help\t\t\tPrints this help message" << endl;
        exit(0);
    }
}
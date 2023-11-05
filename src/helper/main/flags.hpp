#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../../lexer/tokens.hpp"
#include "../../opCode/chunk.hpp"
#include "../../debug/debug.hpp"
#include "./getCurrentTime.hpp"
#include "../../common.hpp"
#include "../../vm/vm.hpp"
#include "./version.hpp"
#include "./repl.hpp"

using namespace std;

static char *read_file(const char *path) {

  if (!path) {
    cout << "+-----------------------------------------+ \n"
         << "| No file path was provided, opening REPL | \n"
         << "+-----------------------------------------+" << endl;
    repl(0, NULL);
    cout << "REPL is not implemented yet" << endl;
  }

  ifstream file(path, ios::binary);
  if (!file) {
    cerr << "Could not open file \"" << path << "\"." << endl;
    Zura::Exit(Zura::Exit_Value::INVALID_FILE);
  }

  file.seekg(0, ios::end);
  size_t file_size = file.tellg();
  file.seekg(0, ios::beg);

  char *buffer = new char[file_size + 1];
  file.read(buffer, file_size);
  file.close();

  buffer[file_size] = '\0';
  return buffer;
}

inline void run_file(const char *path) {
    // !NOTE: consider changing the char* path to use the cpp in built filesystem
    const char* source = read_file(path);

    // Check to make sure that we have  a .zu file extension
    if (strcmp(path + strlen(path) - 3, ".zu") != 0) {
        cerr << "File \"" << path << "\" does not have a .zu extension." << endl;
        Zura::Exit(Zura::Exit_Value::INVALID_FILE_EXTENSION);
    }

    Zura::Exit_Value result = interpret(source);
    delete[] source;

    if (result == Zura::Exit_Value::COMPILATION_ERROR) Zura::Exit(Zura::Exit_Value::COMPILATION_ERROR);
    if (result == Zura::Exit_Value::RUNTIME_ERROR) Zura::Exit(Zura::Exit_Value::RUNTIME_ERROR);
}

inline void flags(int argc, char *argv[]) {

  if (argc == 2 && strcmp(argv[1], "--help") == 0) {
    cout << "Usage: " << argv[0] << " [options]" << endl;
    cout << "Options:" << endl;
    cout << "  --help\t\t\tPrints this help message" << endl;
    cout << "  --version\t\t\tPrints the version of the compiler" << endl;
    cout << "  --license\t\t\tPrints the license of the Zura Lang" << endl;
    Zura::Exit(Zura::Exit_Value::OK);
  }

  if (argc == 2 && strcmp(argv[1], "--version") == 0) {
    cout << get_Zura_version_string() << "(" << getCurrentTime() << ")" << endl;
    Zura::Exit(Zura::Exit_Value::OK);
  }
  if (argc == 2 && strcmp(argv[1], "--license") == 0) {
    cout << "Zura Lang is licenesed under GPL-3.0 "
            "license(https://www.gnu.org/licenses/gpl-3.0.en.html) "
         << endl;
    Zura::Exit(Zura::Exit_Value::OK);
  }
}
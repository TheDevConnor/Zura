#include <iostream>
#include <stdarg.h>
#include "../lib/colorize.hpp"
#include "../parser/lexer/tokens.h"
#include "../vm/vm.h"

inline void error_parser(Token line_no, int pos, const char *msg){
    cout << "\n[" << termcolor::yellow << "line: " << termcolor::cyan << line_no.line << termcolor::reset << "] "
         << "[" << termcolor::yellow << "pos: " << termcolor::cyan << pos - 1 << termcolor::reset << "] Error: ";

    if (line_no.kind == EOF_TOKEN)
        printf("at end \n");
    else if (line_no.kind == ERROR_TOKEN)
    {
    } // Nothing
    else
        cout << "at " << termcolor::red << "'" << termcolor::reset << termcolor::cyan << line_no.length << termcolor::reset << termcolor::red << "'" << termcolor::reset << "\n";

    const char *line_start = get_source_line_start(line_no.line);
    const char *line_end = line_start;
    while (*line_end != '\n' && *line_end != '\0')
        line_end++;

    // Print the line
    cout << termcolor::magenta << string(line_start - 1, line_end - line_start) << termcolor::reset << "\n";

    // Print to the error
    int num_spaces = pos - 2;
    for (int i = 0; i < num_spaces; i++)
        cout << " ";
    cout << termcolor::red << "^" << termcolor::reset << " ";

    // Print the error message
    cout << msg << "\n";
}

// runtime error
inline void runtimeError(const char *format, ...){
    size_t instruction = vm.frames->ip - vm.frames->closure->function->chunk.code - 1;
    int line = vm.frames->closure->function->chunk.lines[instruction];
    if (vm.frames->closure->function->name != nullptr)
    {
        cout << "[" << termcolor::yellow << "line" << termcolor::reset << " -> "
             << termcolor::red << line << termcolor::reset << "][" << termcolor::yellow
             << "pos" << termcolor::reset << " -> " << termcolor::red << instruction
             << termcolor::reset << "][" << termcolor::red << "func" << termcolor::reset
             << " -> " << termcolor::red << vm.frames->closure->function->name->chars
             << termcolor::reset << "] in script \n";
    }
    else
    {
        cout << "[" << termcolor::yellow << "line" << termcolor::reset << " -> "
             << termcolor::red << line << termcolor::reset << "][" << termcolor::yellow
             << "pos" << termcolor::reset << " -> " << termcolor::red << instruction
             << termcolor::reset << "] in script \n";
    }
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    exit(1);
}

#include <iostream>
#include <fstream>
#include <sstream>

#include "debug/debug.h"
#include "parser/chunk.h"
#include "parser/vm.h"
#include "flags.h"

int main(int argc, char* argv[]) {
    flags(argc, argv);

    init_vm();

    Chunk chunk;
    init_chunk(&chunk);

    int constant = add_constant(&chunk, 1.2);
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, constant, 123);

    // Add
    constant = add_constant(&chunk, 5.6);
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, constant, 123);

    write_chunk(&chunk, OP_ADD, 123); // 1.2 + 5.6 = 6.8 % 2 = 0.8

    // Modulo
    constant = add_constant(&chunk, 2.0);
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, constant, 123);

    write_chunk(&chunk, OP_MODULO, 123); // 6.8 % 2 = 0.8

    write_chunk(&chunk, OP_NEGATE, 123);

    write_chunk(&chunk, OP_RETURN, 123);

    disassemble_chunk(&chunk, "test chunk");
    interpret(&chunk);
    free_vm();
    free_chunk(&chunk);

    return 0;
}
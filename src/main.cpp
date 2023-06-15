#include <iostream>
#include <fstream>
#include <sstream>

#include "debug/debug.h"
#include "parser/chunk.h"

int main() {
    Chunk chunk;
    init_chunk(&chunk);

    int constant = add_constant(&chunk, 1.2);
    write_chunk(&chunk, OP_CONSTANT, 123);
    write_chunk(&chunk, constant, 123);

    write_chunk(&chunk, OP_RETURN, 123);

    disassemble_chunk(&chunk, "test chunk");

    free_chunk(&chunk);

    return 0;
}
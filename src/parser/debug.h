#ifndef AZUA_DEBUG_H
#define AZUA_DEBUG_H

#include "../parser/chunk.h"

void disassemble_chunk(Chunk* chunk, const char* name);
int disassemble_instruction(Chunk* chunk, int offset);

#endif // AZUA_DEBUG_H

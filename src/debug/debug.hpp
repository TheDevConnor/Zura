#pragma once

#include "../opCode/chunk.hpp"

void disassembleChunk(Chunk* chunk, const char* name);
int  disassembleInstruction(Chunk* chunk, int offset);
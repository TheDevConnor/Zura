#pragma once

#include "../opCode/chunk.hpp"

struct VM {
    Chunk* chunk;
};

void initVM();
void freeVM();
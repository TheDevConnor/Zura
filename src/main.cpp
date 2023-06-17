#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug/debug.h"
#include "parser/chunk.h"
#include "parser/vm.h"
#include "flags.h"

int main(int argc, char* argv[]) {
    flags(argc, argv);
    init_vm();

    run_file(argv[1]);
    
    free_vm();
    return 0;
}
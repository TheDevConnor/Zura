// Including necessary C standard libraries
#include <ctype.h>
#include <stdio.h>  // Standard input/output functions
#include <stdlib.h> // Standard library functions, e.g., memory allocation
#include <string.h> // String manipulation functions

// Including language headers
#include "common.h"
#include "debug/debug.h"  // Debugging utilities
#include "helper/flags.h" // Command-line flags parsing
#include "parser/chunk.h" // Chunk data structure and parsing functions
#include "vm/vm.h"        // Virtual machine implementation

#if ZURA_GUI
#include "gui/zura_console.h"
const bool run_zura_gui = true;
#else
const bool run_zura_gui = false;
#endif // ZURA_GUI

int main(int argc, char* argv[])
{
    Zura_Exit_Value status = OK;
    
    if(run_zura_gui){

        status = (Zura_Exit_Value)zura_gui_main(argc, argv);

    }else{

        // Init Zura
        flags(argc, argv);
        init_vm();

        // Run Zura
        run_file(argv[1]);

        // Cleanup Zura
        free_vm();
    }

    return (int)status;
}



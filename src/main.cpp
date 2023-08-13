// Including necessary C standard libraries
#include <stdio.h>  // Standard input/output functions
#include <stdlib.h> // Standard library functions, e.g., memory allocation
#include <string.h> // String manipulation functions

// Including language headers
#include "debug/debug.h"  // Debugging utilities
#include "helper/flags.h" // Command-line flags parsing
#include "parser/chunk.h" // Chunk data structure and parsing functions
#include "vm/vm.h"        // Virtual machine implementation

int main(int argc, char *argv[]) {
  // Parsing command-line arguments using custom 'flags' function
  flags(argc, argv);

  // Initializing the virtual machine
  init_vm();

  // Running the code in the file specified in the command-line argument
  // (argv[1])
  run_file(argv[1]);

  // Freeing resources and cleaning up the virtual machine
  free_vm();

  // The main function should return 0 to indicate successful execution
  return 0;
}

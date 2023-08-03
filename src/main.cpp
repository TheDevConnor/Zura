/******************************************************************************
 * This file is part of Zura.
 *
 * Zura is free software: you may only use it under the terms of the
 * GNU General Public License as published by
 * the Free Software Foundation, of version 3 of the License, or
 * (at your option) any later version.
 *
 * Zura is distributed, WITHOUT ANY WARRANTY "AS IS" 
 * without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Zura. If not, see <https://www.gnu.org/licenses/gpl-3.0.html>.
 *****************************************************************************/

// Including necessary C standard libraries
#include <stdio.h>    // Standard input/output functions
#include <stdlib.h>   // Standard library functions, e.g., memory allocation
#include <string.h>   // String manipulation functions

// Including language headers
#include "debug/debug.h"    // Debugging utilities
#include "parser/chunk.h"   // Chunk data structure and parsing functions
#include "vm/vm.h"          // Virtual machine implementation
#include "helper/flags.h"   // Command-line flags parsing

int main(int argc, char *argv[])
{
    // Parsing command-line arguments using custom 'flags' function
    flags(argc, argv);

    // Initializing the virtual machine
    init_vm();

    // Running the code in the file specified in the command-line argument (argv[1])
    run_file(argv[1]);

    // Freeing resources and cleaning up the virtual machine
    free_vm();

    // The main function should return 0 to indicate successful execution
    return 0;
}
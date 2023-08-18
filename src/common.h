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
#pragma once

#include <stdint.h>

#define DEBUG_STRESS_GC
#define DEBUG_LOG_GC

// #define NAN_BOXING

#define DEBUG_TRACE_EXECUTION
#define DEBUG_PRINT_CODE

#define UINT8_COUNT (UINT8_MAX + 1)



/// Exit codes used in the Zura Interpreter.
///
/// Add additional exit codes to this list whenever an appropriate
/// reason is encountered to do so. 
///
/// @sa ZuraExit
typedef enum {

    OK = 0,

    INVALID_FILE_EXTENSION = 10,
    INVALID_FILE           = 11,
    COMPILATION_ERROR      = 12,
    RUNTIME_ERROR          = 13,
    BAD_GRAY_STACK         = 14,
    MEMORY_FAILURE         = 15,
    VM_ERROR               = 16

} Zura_Exit_Value;

/// Exits the Zura interpreter using an appropriate exit value.
/// @param exit_value An exit value specific to the Zura interpreter code base.
/// @sa Zura_Exit_Value
void ZuraExit(Zura_Exit_Value exit_value);

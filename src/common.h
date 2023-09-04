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
typedef enum : int {

    OK = 0,

    INVALID_FILE_EXTENSION = 10,
    INVALID_FILE           = 11,
    COMPILATION_ERROR      = 12,
    RUNTIME_ERROR          = 13,
    BAD_GRAY_STACK         = 14,
    MEMORY_FAILURE         = 15,
    VM_ERROR               = 16,
    UNABLE_TO_INIT_GUI     = 17,
    FAILED_TO_START_IMGUI  = 18

} Zura_Exit_Value;

/// Exits the Zura interpreter using an appropriate exit value.
/// @param exit_value An exit value specific to the Zura interpreter code base.
/// @sa Zura_Exit_Value
void ZuraExit(Zura_Exit_Value exit_value);

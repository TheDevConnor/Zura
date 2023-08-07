/* 
    LOGGER NATIVE FUNCTIONS

    - Our goal for this logger is to have it be a class that is exported so we can pass in options to it

    - We will have a few different levels of logging:
        - Info
        - Debug
        - Warn
        - Error
        - Critical Error

    - We will also have a few different options for the logger:
        - File logging : boolean
            - File path(s) (both relative and absolute paths) : string
        - Console logging : boolean
*/

#pragma once 

#include <cstring>
#include <ctime>
#include <cmath>
#include <fstream>

#include "../../compiler/object.h"
#include "../../vm/vm.h"
#include "../define_native.h"


class Logger {


    private:
    
        static Value critical_error(int arg_count, Value* args) {
            if (arg_count != 1) return BOOL_VAL(false);
            if (!IS_STRING(args[0])) return BOOL_VAL(false);

            std::cout << "[CRITICAL ERROR] " <<  AS_CSTRING(args[0]) << std::endl;
            return BOOL_VAL(true);            
        }

        static Value error_native(int arg_count, Value* args) {
            if (arg_count != 1) return BOOL_VAL(false);
            if (!IS_STRING(args[0])) return BOOL_VAL(false);

            std::cout << "[ERROR] " <<  AS_CSTRING(args[0]) << std::endl;
            return BOOL_VAL(true);            
        }

        static Value warn_native(int arg_count, Value* args) {             
            if (arg_count != 1) return BOOL_VAL(false);
            if (!IS_STRING(args[0])) return BOOL_VAL(false);

            std::cout << "[WARN] " <<  AS_CSTRING(args[0]) << std::endl;
            return BOOL_VAL(true);
        }

        static Value debug_native(int arg_count, Value* args) {
            if (arg_count != 1) return BOOL_VAL(false);
            if (!IS_STRING(args[0])) return BOOL_VAL(false);    

            std::cout << "[Debug] " <<  AS_CSTRING(args[0]) << std::endl;
            return BOOL_VAL(true);            
        }

        static Value info_native(int arg_count, Value* args) {
            if (arg_count != 1) return BOOL_VAL(false);
            if (!IS_STRING(args[0])) return BOOL_VAL(false);

            std::cout << "[INFO] " <<  AS_CSTRING(args[0]) << std::endl;
            return BOOL_VAL(true);
        }
 
    public:
        static void define_logger_natives() {
            Natives::define_native("lgInfo", info_native);
            Natives::define_native("lgDebug", debug_native);
            Natives::define_native("lgWarn", warn_native);
            Natives::define_native("lgError", error_native);
            Natives::define_native("lgCriticalError", critical_error);
        }
};

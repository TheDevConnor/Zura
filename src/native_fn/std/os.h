#pragma once 

#include <iostream>
#include <cstring>
#include <ctime>
#include <cmath>

#include "../../compiler/object.h"
#include "../../vm/vm.h"
#include "../define_native.h"

class Os {
private:
    static Value execute_command_native(int arg_count, Value* args) {
        if (arg_count != 1) {
            std::cout << "os_execute_command_native: Expected 1 argument, got " << arg_count << std::endl;
            exit(1);
        }
        std::string command = AS_CSTRING(args[0]);
        int result = system(command.c_str());
        return NUMBER_VAL(result);
    }

    static Value http_request_native(int arg_count, Value* args) {
        if (arg_count != 1) {
            std::cout << "os_http_request_native: Expected 1 argument, got " << arg_count << std::endl;
            exit(1);
        }
        std::string url = AS_CSTRING(args[0]);
        std::string command = "curl " + url;
        int result = system(command.c_str());
        return NUMBER_VAL(result);
    }
    static Value http_get_native(int arg_count, Value* args) {
        if (arg_count != 1) {
            std::cout << "os_http_get_native: Expected 1 argument, got " << arg_count << std::endl;
            exit(1);
        }
        std::string url = AS_CSTRING(args[0]);
        std::string command = "curl -X GET " + url;
        int result = system(command.c_str());
        return NUMBER_VAL(result);
    }
    static Value download_file_native(int arg_count, Value* args) {
        if (arg_count != 2) {
            std::cout << "os_download_file_native: Expected 2 arguments, got " << arg_count << std::endl;
            exit(1);
        }
        std::string url = AS_CSTRING(args[0]);
        std::string filename = AS_CSTRING(args[1]);
        std::string command = "curl -o " + filename + " " + url;
        int result = system(command.c_str());
        return NUMBER_VAL(result);
    }

public:
    static void define_os_natives() {
        Natives::define_native("osExecuteCommand", execute_command_native);
    
        Natives::define_native("osHttpRequest", http_request_native);
        Natives::define_native("osHttpGet", http_get_native);
        Natives::define_native("osDownloadFile", download_file_native);
    }
};
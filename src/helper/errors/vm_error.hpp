#pragma once

#include <iostream>
#include <string>

#include "../../../inc/colorize.hpp"
#include "../../common.hpp"

class VMError {
private:
public:
    static void vm_error(std::string message) {
        std::cout << termcolor::red << "VM Error: " << termcolor::reset << message << std::endl;
        ZuraExit(VM_ERROR);
    }
};
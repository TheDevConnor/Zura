#include <iostream>
#include <fstream>

#include "helper/main/flags.hpp"
#include "common.hpp"
#include "vm/vm.hpp"

int main(int argc, char* argv[]) {
    initVM();

    flags(argc, argv);
    run_file(argv[1]);
    
    freeVM();
    return 0;
}   

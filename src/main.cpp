#include <iostream>
#include <fstream>

#include "helper/main/flags.h"
#include "common.hpp"

int main(int argc, char* argv[]) {
    flags(argc, argv);

    run_file(argv[1]);

    return 0;
}   
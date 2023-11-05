#pragma once

#define REPL_H

#include "getCurrentTime.hpp"
#include "../../vm/vm.hpp"
#include "version.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

inline int repl(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    std::cout << "Zura Lang REPL " << get_Zura_version_string() << " "
              << "(" << getCurrentTime() << ")" << std::endl;
              
    char line[1024];
    while (true) {
    std::cout << ">>> ";
    if (!fgets(line, sizeof(line), stdin)) break;
    interpret(line);
    }

    return 0;
}

#ifndef REPL_H

#define REPL_H

#include "getCurrentTime.h"
#include "version.h"
#include "../parser/vm.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int repl(int argc, char *argv[])
{

    std::cout << "Zura Lang REPL " << version() << " "
              << "(" << getCurrentTime() << ")" << std::endl;

    char line[1024];
    for (;;)
    {
        std::cout << "> ";
        if (!fgets(line, sizeof(line), stdin))
            break;
        interpret(line);
    }
}

#endif // REPL_H
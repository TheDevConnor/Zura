#ifndef REPL_H
#define REPL_H

#include "../vm/vm.h"
#include "getCurrentTime.h"
#include "version.h"
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
  for (;;) {
    std::cout << ">>> ";
    if (!fgets(line, sizeof(line), stdin))
      break;
    interpret(line);
  }

  return 0;
}

#endif // REPL_H

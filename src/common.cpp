#include <stdlib.h>
#include "common.hpp"

namespace Zura {

/// Zura::Exit_Value is scoped to type int
void Exit(Exit_Value exit_value)
{
    exit( static_cast<int>(Exit_Value(exit_value)) );    
}

}

#include <stdlib.h>
#include "common.h"

void ZuraExit(Zura_Exit_Value exit_value)
{
    exit( Zura_Exit_Value(exit_value) );    
}

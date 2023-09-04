#include <string>
#include <sstream>

#include "version.h"

std::string get_Zura_version_string(void) 
{
    Zura_version v = {};
    ZURA_VERSION(&v);

    std::string version;
    std::ostringstream build_version;

    build_version << v.major << '.' << v.minor << '.' << v.patch;
    
    version = build_version.str();

    return version;
}

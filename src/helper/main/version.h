#ifndef VERSION_H
#define VERSION_H

#include <string>
#include <sstream>

const int ZURA_MAJOR_VERSION = 2;
const int ZURA_MINOR_VERSION = 0;
const int ZURA_PATCH_LEVEL   = 1;

typedef struct {

    int major;
    int minor;
    int patch;

} Zura_version;

/// Macro to determine Zura version.
/// @param x A pointer to a Zura_version struct to initialize.
#define ZURA_VERSION(x)               \
{                                     \
    (x)->major = ZURA_MAJOR_VERSION;  \
    (x)->minor = ZURA_MINOR_VERSION;  \
    (x)->patch = ZURA_PATCH_LEVEL;    \
}

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


#endif // VERSION_H

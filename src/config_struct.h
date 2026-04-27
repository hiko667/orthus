#ifndef CONFIG_STRUCT_H
#define CONFIG_STRUCT_H
#include <stdbool.h>

struct configStruct
{
    char * sourceDir;
    char * targetDir;
    int awakeningFrequency; 
    bool recursive;
    long long minSizeToBeBig;
};

#endif
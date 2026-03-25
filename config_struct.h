#ifndef CONFIG_STRUCT_H
#define CONFIG_STRUCT_H
#include <stdbool.h>
struct configStruct
{
    char sourceDir[128];
    char targetDir[128];
    int awakeningFrequency; 
    bool recursive;
    int minSizeToBeBig;
};

enum options
{
    SOURCE,
    TARGET,
    FREQUENCY,
    RECURSIVE,
    MINSIZE,
    ERROR
};  

float version = 0.1;
#endif
#ifndef FILE_STRUCT_H
#define FILE_STRUCT_H
#include <time.h>
struct fileStruct{
    char * fileName[256];
    struct timespec lastModified;
};
#endif
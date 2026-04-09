#ifndef FILE_STRUCT_H
#define FILE_STRUCT_H

#include <time.h>
struct fileStruct{
    char * fileName;
    struct timespec lastModified;
};
#endif
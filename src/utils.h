#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include <sys/stat.h>

bool is_dir(const char *path);
int countFiles(const char * path);

#endif 
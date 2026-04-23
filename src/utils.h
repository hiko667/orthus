#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include <sys/stat.h>
char *BuildPath(const char *dir, const char *name);
bool IsDir(const char *path);
int CountFiles(const char * path);
int DaemonizeProcess();
bool CopyFile(const char *srcPath, const char *dstPath);
bool RemovePath(const char *path);
#endif 
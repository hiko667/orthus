#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include <sys/stat.h>
char *build_path(const char *dir, const char *name);
bool is_dir(const char *path);
int countFiles(const char * path);
int daemonize_process();
bool copy_file(const char *srcPath, const char *dstPath);
bool remove_path(const char *path);
#endif 
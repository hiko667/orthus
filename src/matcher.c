#define _DEFAULT_SOURCE // see utils.c
#include "matcher.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>
char ** getFileList(const char * path)
{
    int countedFiles = countFiles(path);
    DIR * dir = opendir(path);
    if(!dir || countedFiles == -1) return NULL;
    char ** files = malloc(sizeof(char*) * countedFiles);
    struct dirent * entry;
    int count;
    while((entry = readdir(dir)) != NULL)
    {
        if(entry->d_type = DT_REG)
        {
            files[count] = strdup(entry->d_name);
            count++;
        }
    }
    closedir(dir);
    return files;
}
void freeFiles(char ** files)
{
    int count = sizeof(files) / sizeof(char*);
    for(int i = 0; i<count; i++) free(files[i]);
    free(files);
}
bool match(struct configStruct * configurations)
{
    
}
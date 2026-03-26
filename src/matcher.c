#define _DEFAULT_SOURCE // see utils.c
#include "matcher.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>
#include "file_struct.h"
struct fileStruct ** getFileList(const char * path)
{
    int countedFiles = countFiles(path);
    DIR * dir = opendir(path);
    if(!dir || countedFiles == -1) return NULL;
    struct fileStruct ** files = malloc(sizeof(struct fileStruct *) * countedFiles);
    for(int i = 0; i<countedFiles; i++) files[i] = malloc(sizeof(struct fileStruct *));
    struct dirent * entry;
    int count;
    while((entry = readdir(dir)) != NULL)
    {
        if(entry->d_type = DT_REG)
        {
            files[count]->fileName = strdup(entry->d_name);
            count++;
        }
    }
    closedir(dir);
    return files;
}
void freeFiles(struct fileStruct ** files)
{
    int count = sizeof(files) / sizeof(struct fileStruct*);
    for(int i = 0; i<count; i++) free(files[i]);
    free(files);
}
bool match(struct configStruct * configurations)
{
    
}
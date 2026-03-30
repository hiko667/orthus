#define _DEFAULT_SOURCE // see utils.c
#include "matcher.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>
#include "file_struct.h"

struct fileStruct ** getFileList(const char * path, int * counted)
{
    int countedFiles = countFiles(path);
    * counted = countedFiles;
    DIR * dir = opendir(path);
    if(!dir || countedFiles == -1) return NULL;
    struct fileStruct ** files = malloc(sizeof(struct fileStruct *) * countedFiles);
    for(int i = 0; i<countedFiles; i++) files[i] = malloc(sizeof(struct fileStruct));
    struct dirent * entry;
    struct stat st;
    int count = 0;
    while((entry = readdir(dir)) != NULL)
    {
        if(entry->d_type == DT_REG)
        {
            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
            files[count]->fileName = strdup(entry->d_name);
            stat(entry->d_name, &st);
            files[count]->lastModified = st.st_mtim;
            count++;
        }
    }
    closedir(dir);
    return files;
}

char ** getPathsToCopy(struct configStruct configurations)
{
    int sourceCount, targetCount;
    struct fileStruct ** sourceFiles = getFileList(configurations.sourceDir, &sourceCount);
    struct fileStruct ** targetFiles = getFileList(configurations.targetDir, &targetCount);

    freeFiles(sourceFiles, sourceCount);
    freeFiles(targetFiles, targetCount);
}

void freeFiles(struct fileStruct ** files, int howMany)
{
    for(int i = 0; i<howMany; i++) free(files[i]);
    free(files);
}

bool match(struct configStruct * configurations)
{

}
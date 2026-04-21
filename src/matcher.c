#define _DEFAULT_SOURCE // see utils.c
#include "matcher.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>
#include <sys/stat.h>
#include "logs.h"

static int name_exists_in_list(char **list, int count, const char *name)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(list[i], name) == 0)
            return 1;
    }
    return 0;
}

char ** getFileList(const char *path, int *outCount)
{
    int countedFiles = countFiles(path);
    * counted = countedFiles;
    DIR * dir = opendir(path);
    if(!dir || countedFiles == -1) return NULL;
    struct fileStruct ** files = malloc(sizeof(struct fileStruct *) * countedFiles);
    for(int i = 0; i<countedFiles; i++) files[i] = malloc(sizeof(struct fileStruct));
    struct dirent * entry;
    int count=0;
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
    *outCount = count;
    return files;
}

static void freeFiles(char **files, int count)
{
    if (!files)
        return;

    for (int i = 0; i < count; i++)
        free(files[i]);

    free(files);
}

bool match(struct configStruct * configurations)
{
    int sourceCount = 0;
    int targetCount = 0;

    char **sourceFiles = getFileList(configurations->sourceDir, &sourceCount);
    char **targetFiles = getFileList(configurations->targetDir, &targetCount);

    if (!sourceFiles || !targetFiles || sourceCount < 0 || targetCount < 0)
    {
        freeFiles(sourceFiles, sourceCount);
        freeFiles(targetFiles, targetCount);
        return false;
    }

    for (int i = 0; i < sourceCount; i++)
    {
        char *srcPath = build_path(configurations->sourceDir, sourceFiles[i]);
        char *dstPath = build_path(configurations->targetDir, sourceFiles[i]);

        if (!srcPath || !dstPath)
        {
            free(srcPath);
            free(dstPath);
            continue;
        }

        struct stat srcStat;
        struct stat dstStat;

        if (lstat(srcPath, &srcStat) != 0)
        {
            free(srcPath);
            free(dstPath);
            continue;
        }

        if (!S_ISREG(srcStat.st_mode))
        {
            free(srcPath);
            free(dstPath);
            continue;
        }

        int needCopy = 0;
        if (lstat(dstPath, &dstStat) != 0)
            needCopy = 1;
        else if (srcStat.st_mtime > dstStat.st_mtime)
            needCopy = 1;

        if (needCopy)
        {
            if (copy_file(srcPath, dstPath))
            {
                systemLog(sourceFiles[i], COPIED);
            }
        }

        free(srcPath);
        free(dstPath);
    }

    for (int i = 0; i < targetCount; i++)
    {
        if (!name_exists_in_list(sourceFiles, sourceCount, targetFiles[i]))
        {
            char *dstPath = build_path(configurations->targetDir, targetFiles[i]);
            if (!dstPath)
                continue;

            if (remove_path(dstPath))
            {
                systemLog(targetFiles[i], DELETED);
            }

            free(dstPath);
        }
    }

    freeFiles(sourceFiles, sourceCount);
    freeFiles(targetFiles, targetCount);
    return true;
}
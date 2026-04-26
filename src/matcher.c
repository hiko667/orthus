#define _DEFAULT_SOURCE // see utils.c
#include "matcher.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <string.h>
#include <sys/stat.h>
#include "logs.h"

static int NameExistsInList(char **list, int count, const char *name)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(list[i], name) == 0)
            return 1;
    }
    return 0;
}

char **GetFileList(const char *path, int *outCount)
{
    int countedFiles = CountFiles(path);
    DIR *dir = opendir(path);
    if (!dir || countedFiles == -1)
        return NULL;
    if (countedFiles == 0)
    {
        closedir(dir);
        *outCount = 0;
        return NULL;
    }
    char **files = malloc(sizeof(char *) * countedFiles);
    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            if (entry->d_name[0] == '.')
                continue;
            files[count] = strdup(entry->d_name);
            count++;
        }
    }
    closedir(dir);
    *outCount = count;
    return files;
}

static void FreeFiles(char **files, int count)
{
    if (!files)
        return;

    for (int i = 0; i < count; i++)
        free(files[i]);

    free(files);
}

bool Match(struct configStruct *configurations)
{
    int sourceCount = 0;
    int targetCount = 0;

    char **sourceFiles = GetFileList(configurations->sourceDir, &sourceCount);
    char **targetFiles = GetFileList(configurations->targetDir, &targetCount);

    if (sourceCount < 0 || targetCount < 0)
    {
        FreeFiles(sourceFiles, sourceCount);
        FreeFiles(targetFiles, targetCount);
        return false;
    }

    for (int i = 0; i < sourceCount; i++)
    {
        char *srcPath = BuildPath(configurations->sourceDir, sourceFiles[i]);
        char *dstPath = BuildPath(configurations->targetDir, sourceFiles[i]);

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
            if (CopyFile(srcPath, dstPath, configurations->minSizeToBeBig))
            {
                SystemLog(sourceFiles[i], COPIED);
            }
        }

        free(srcPath);
        free(dstPath);
    }

    for (int i = 0; i < targetCount; i++)
    {
        if (!NameExistsInList(sourceFiles, sourceCount, targetFiles[i]))
        {
            char *dstPath = BuildPath(configurations->targetDir, targetFiles[i]);
            if (!dstPath)
                continue;

            if (RemovePath(dstPath))
            {
                SystemLog(targetFiles[i], DELETED);
            }

            free(dstPath);
        }
    }

    FreeFiles(sourceFiles, sourceCount);
    FreeFiles(targetFiles, targetCount);
    return true;
}

bool RecursiveMatch(struct configStruct *configurations)
{
    DIR *dir = opendir(configurations->sourceDir); //otwarcie katalogu
    if (!dir)
    {
        SystemLog("Cannot open source directory", ASLEEP);
        return false;
    }

    // wykonanie dopasowania na zwykłych plikach
    if (!Match(configurations))
    {
        closedir(dir);
        return false;
    }

    // obsługa podkatalogów
    struct dirent *entry;
    rewinddir(dir); // przewiń katalog od nowa

    while ((entry = readdir(dir)) != NULL) //przechodzi po katalogach i pomija pliki
    {
        if (entry->d_name[0] == '.')
            continue;
        if (entry->d_type != DT_DIR)
            continue;

        // Zbuduj ścieżki dla podkatalogu
        char *srcSubDir = BuildPath(configurations->sourceDir, entry->d_name);
        char *dstSubDir = BuildPath(configurations->targetDir, entry->d_name);

        if (!srcSubDir || !dstSubDir)
        {
            free(srcSubDir);
            free(dstSubDir);
            continue;
        }

        // tworzy katalog docelowy jeśli nie istnieje
        struct stat st;
        if (stat(dstSubDir, &st) != 0)
        {
            if (mkdir(dstSubDir, 0755) != 0)
            {
                SystemLog(entry->d_name, DELETED); 
                free(srcSubDir);
                free(dstSubDir);
                continue;
            }
        }

        // Rekurencyjne wywołanie dla podkatalogu
        struct configStruct subConfig = *configurations; // kopia
        strcpy(subConfig.sourceDir, srcSubDir);
        strcpy(subConfig.targetDir, dstSubDir);

        RecursiveMatch(&subConfig);

        free(srcSubDir);
        free(dstSubDir);
    }

    // Usuń katalogi z docelowego które nie istnieją w źródle
    // i trzeba sprawdzać czy metadane są inne żaby za każdym razem nie kopiowac tego samego jeśli już jest
    closedir(dir);
    return true;
}
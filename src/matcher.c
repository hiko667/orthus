#define _DEFAULT_SOURCE // see utils.c
#include "matcher.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <utime.h>
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
    SystemLog("Daemon awoke. Begining to match", AWOKE);
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
    SystemLog("Daemon awoke. Begining to match recursively", AWOKE);
    DIR *dir = opendir(configurations->sourceDir); //otiwra katalog
    if (!dir)
    {
        SystemLog("Cannot open source directory", ASLEEP);
        SystemLog(configurations->sourceDir, ASLEEP);
        return false;
    }

    // Porównaj czasy modyfikacji KATALOGÓW (źródłowego i docelowego)
    struct stat srcDirStat, dstDirStat;

    if (stat(configurations->sourceDir, &srcDirStat) != 0) //próba pobrania danych o źródle
    {
        closedir(dir);
        return false;
    }

    //  katalog docelowy istnieje i ma ten sam czas co źródłowy
    if (stat(configurations->targetDir, &dstDirStat) == 0)
    {
        if (srcDirStat.st_mtime <= dstDirStat.st_mtime)
        {
            // Katalog nie był modyfikowany od ostatniej synchronizacji
            closedir(dir);
            return true;
        }
    }

    // Wykonaj Match na plikach
    if (!Match(configurations))
    {
        closedir(dir);
        return false;
    }

    // Obsługa podkatalogów
    struct dirent *entry;
    rewinddir(dir);

    while ((entry = readdir(dir)) != NULL) //nie jest to katalog ukryty lub plik
    {
        if (entry->d_name[0] == '.')
            continue;
        if (entry->d_type != DT_DIR)
            continue;

        char *srcSubDir = BuildPath(configurations->sourceDir, entry->d_name);
        char *dstSubDir = BuildPath(configurations->targetDir, entry->d_name);

        if (!srcSubDir || !dstSubDir)
        {
            free(srcSubDir);
            free(dstSubDir);
            continue;
        }

        // Tworzy katalog docelowy jeśli nie istnieje
        struct stat st;
        if (stat(dstSubDir, &st) != 0)
        {
            //tworzy dolder z takimi uprawnieniami 777 
            //jeśli jest to możliwe lub takimi na jakie zezwoli linux,
            //możecie wykminić jak nadać takie same uprawnienia jak folder źródłowy
            if (mkdir(dstSubDir, 0777) != 0) 
            {
                SystemLog(entry->d_name, DELETED);
                free(srcSubDir);
                free(dstSubDir);
                continue;
            }
        }

        // Rekurencyjne wywołanie 
        struct configStruct subConfig = *configurations;
        strcpy(subConfig.sourceDir, srcSubDir);
        strcpy(subConfig.targetDir, dstSubDir);

        RecursiveMatch(&subConfig);

        free(srcSubDir);
        free(dstSubDir);
    }

    //tu dopisać funkcje co będzie usuwać pliki co są w folderze docelowym ale nie ma ich w źródłowm
    //CleanUpDirectories(configurations);

    // ustaiwenei czasu na źródłowy
    struct utimbuf times;
    times.actime = srcDirStat.st_atime;
    times.modtime = srcDirStat.st_mtime;
    utime(configurations->targetDir, &times);

    closedir(dir);
    return true;
}
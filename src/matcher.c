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

static bool NameExistsInList(char **list, int count, const char *name)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(list[i], name) == 0)
            return 1;
    }
    return 0;
}

// Returns a list of file names and saves their number to the outCount variable
char **GetFileList(const char *path, int *outCount)
{
    int countedFiles = CountFiles(path);
    DIR *dir = opendir(path);
    // Return NULL if the path doesn't lead to a directory or countedFiles returns an error
    if (!dir || countedFiles == -1)
        return NULL;
    // Return Null if no files are in the directry
    if (countedFiles == 0)
    {
        closedir(dir);
        *outCount = 0;
        return NULL;
    }
    char **files = malloc(sizeof(char *) * countedFiles);
    struct dirent *entry;
    int count = 0;
    // Loop through every file in the directory
    while ((entry = readdir(dir)) != NULL)
    {
        // Regular file
        if (entry->d_type == DT_REG)
        {
            // The file is hidden, skip over it
            if (entry->d_name[0] == '.')
                continue;
            // Add the file's name to the result
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

// The function that matches target and source, copies the files if needed and returns true if nothing goes wrong
bool Match(struct configStruct *configurations)
{
    int sourceCount = 0;
    int targetCount = 0;
    char **sourceFiles = GetFileList(configurations->sourceDir, &sourceCount);
    char **targetFiles = GetFileList(configurations->targetDir, &targetCount);
    SystemLog("Began to look for files to copy", ACTION);
    // Directories are empty
    if (sourceCount < 0 || targetCount < 0)
    {
        FreeFiles(sourceFiles, sourceCount);
        FreeFiles(targetFiles, targetCount);
        return false;
    }
    char message [128];
    sprintf(message, "Found %d src %d trg", sourceCount, targetCount);
    SystemLog(message, ACTION);
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

        // If the file isn't regular
        if (!S_ISREG(srcStat.st_mode))
        {
            free(srcPath);
            free(dstPath);
            continue;
        }

        bool needCopy = false;
        // Check if the source file doesn't exist in target
        if (lstat(dstPath, &dstStat) != 0)
            needCopy = true;
        // Check if the date of modification in target file is lesser than source
        else if (srcStat.st_mtime > dstStat.st_mtime)
            needCopy = true;

        if (needCopy)
        {
            SystemLog("Found file that needs copy", ACTION);
            if (CopyFile(srcPath, dstPath, configurations->minSizeToBeBig))
                SystemLog(sourceFiles[i], COPIED);
        }

        free(srcPath);
        free(dstPath);
    }
    SystemLog("Began to look for files to delete", ACTION); 
    // Loop through files in target directory to check which don't exist in source anymore
    for (int i = 0; i < targetCount; i++)
    {
        if (!NameExistsInList(sourceFiles, sourceCount, targetFiles[i]))
        {
            char *dstPath = BuildPath(configurations->targetDir, targetFiles[i]);
            if (!dstPath)
                continue;

            if (RemovePath(dstPath))
                SystemLog(targetFiles[i], DELETED);

            free(dstPath);
        }
    }

    FreeFiles(sourceFiles, sourceCount);
    FreeFiles(targetFiles, targetCount);
    sprintf(message, "Ending iteration for %d", configurations->sourceDir);
    SystemLog(message, ACTION);
    return true;
}

// The function that recursively matches target and source using Match(), copies the files if needed and returns true if nothing goes wrong
bool RecursiveMatch(struct configStruct *configurations)
{
    char message [128];
    sprintf(message, "Begining to match recursively %d ",configurations->sourceDir);
    SystemLog(message, ACTION);
    DIR *dir = opendir(configurations->sourceDir);
    if (!dir)
    {
        sprintf(message, "Cannot open source directory %d ",configurations->sourceDir);
        SystemLog(message, ASLEEP);
        SystemLog(configurations->sourceDir, ASLEEP);
        return false;
    }

    
    struct stat srcDirStat, dstDirStat;
    if (stat(configurations->sourceDir, &srcDirStat) != 0)
    {
        closedir(dir);
        return false;
    }

    // Check if the date of modification in target file is lesser than source
    if (stat(configurations->targetDir, &dstDirStat) == 0)
    {
        if (srcDirStat.st_mtime <= dstDirStat.st_mtime)
        {
            closedir(dir);
            return true;
        }
    }

    // Do Match() on the files inside current directory
    if (!Match(configurations))
    {
        closedir(dir);
        return false;
    }

    // Work on subdirectories now
    SystemLog("Began to look for directories to copy", ACTION);
    struct dirent *entry;
    rewinddir(dir);
    
    while ((entry = readdir(dir)) != NULL)
    {
        // If it is hidden
        if (entry->d_name[0] == '.')
            continue;
        // If it isn't a directory
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

        // Create a target directory if it doesn't exist
        struct stat st;
        if (stat(dstSubDir, &st) != 0)
        {
            // Creates a directory with the permissions of 777
            if (mkdir(dstSubDir, 0777) != 0) 
            {
                SystemLog(entry->d_name, DELETED);
                free(srcSubDir);
                free(dstSubDir);
                continue;
            }
            sprintf(message, "Copied %d directory to target",srcSubDir);
            SystemLog(message, COPIED); 
        }

        // Recursive call
        struct configStruct subConfig = *configurations;
        strcpy(subConfig.sourceDir, srcSubDir);
        strcpy(subConfig.targetDir, dstSubDir);

        RecursiveMatch(&subConfig);

        free(srcSubDir);
        free(dstSubDir);
    }

    // DLA STASIA: tu dopisać funkcje co będzie usuwać pliki co są w folderze docelowym ale nie ma ich w źródłowm
    //CleanUpDirectories(struct configStruct configurations) <– dodać taką fukncje

    
    // Set the target time to source
    struct utimbuf times;
    times.actime = srcDirStat.st_atime;
    times.modtime = srcDirStat.st_mtime;
    utime(configurations->targetDir, &times);

    closedir(dir);
    return true;
}

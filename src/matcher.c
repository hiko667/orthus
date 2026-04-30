#define _DEFAULT_SOURCE // see utils.c
#include "matcher.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include <utime.h>
#include <unistd.h>
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
    char message[128];
    sprintf(message, "Found %d src %d trg", sourceCount, targetCount);
    SystemLog(message, ACTION);
    for (int i = 0; i < sourceCount; i++)
    {
        char *srcPath = BuildPath(configurations->sourceDir, sourceFiles[i]);
        char *trgPath = BuildPath(configurations->targetDir, sourceFiles[i]);

        if (!srcPath || !trgPath)
        {
            free(srcPath);
            free(trgPath);
            continue;
        }

        struct stat srcStat;
        struct stat trgStat;

        if (lstat(srcPath, &srcStat) != 0)
        {
            free(srcPath);
            free(trgPath);
            continue;
        }

        // If the file isn't regular
        if (!S_ISREG(srcStat.st_mode))
        {
            free(srcPath);
            free(trgPath);
            continue;
        }

        bool needCopy = false;
        // Check if the source file doesn't exist in target
        if (lstat(trgPath, &trgStat) != 0)
            needCopy = true;
        // Check if the date of modification in target file is lesser than source
        else if (srcStat.st_mtime > trgStat.st_mtime)
            needCopy = true;

        if (needCopy)
        {
            SystemLog("Found file that needs copy", ACTION);
            if (CopyFile(srcPath, trgPath, configurations->minSizeToBeBig))
                SystemLog(sourceFiles[i], COPIED);
        }

        free(srcPath);
        free(trgPath);
    }
    SystemLog("Began to look for files to delete", ACTION);
    // Loop through files in target directory to check which don't exist in source anymore
    for (int i = 0; i < targetCount; i++)
    {
        if (!NameExistsInList(sourceFiles, sourceCount, targetFiles[i]))
        {
            char *trgPath = BuildPath(configurations->targetDir, targetFiles[i]);
            if (!trgPath)
                continue;

            if (RemovePath(trgPath))
                SystemLog(targetFiles[i], DELETED);

            free(trgPath);
        }
    }

    FreeFiles(sourceFiles, sourceCount);
    FreeFiles(targetFiles, targetCount);
    sprintf(message, "Ending iteration for %s", configurations->sourceDir);
    SystemLog(message, ACTION);
    return true;
}

bool RemoveDirectoryRecursively(const char *path)
{
    DIR *dir = opendir(path);
    if (!dir)
        return false;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;

        char *fullPath = BuildPath(path, entry->d_name);
        struct stat st;
        if (stat(fullPath, &st) == 0)
        {
            if (S_ISDIR(st.st_mode))
                RemoveDirectoryRecursively(fullPath); 
            else
                unlink(fullPath); //deletes file
        }
        free(fullPath);
    }
    closedir(dir);
    return rmdir(path) == 0; //deletes directory
}

// The function that recursively matches target and source using Match(), copies the files if needed and returns true if nothing goes wrong
bool RecursiveMatch(struct configStruct *configurations)
{
    char message[512];
    sprintf(message, "Beginning to match recursively %s", configurations->sourceDir);
    SystemLog(message, ACTION);

    DIR *dir = opendir(configurations->sourceDir);
    if (!dir)
    {
        sprintf(message, "Cannot open source directory %s", configurations->sourceDir);
        SystemLog(message, ASLEEP);
        return false;
    }

    struct stat srcDirStat;
    if (stat(configurations->sourceDir, &srcDirStat) != 0)
    {
        closedir(dir);
        return false;
    }

    struct stat trgDirStat;
    if (stat(configurations->targetDir, &trgDirStat) != 0)
    {
        closedir(dir);
        return false;
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
        if (entry->d_name[0] == '.' || entry->d_type != DT_DIR)
            continue;

        char *srcSubDir = BuildPath(configurations->sourceDir, entry->d_name);
        char *trgSubDir = BuildPath(configurations->targetDir, entry->d_name);

        if (!srcSubDir || !trgSubDir)
        {
            free(srcSubDir);
            free(trgSubDir);
            continue;
        }

        // Create a target directory if it doesn't exist
        struct stat st;
        if (stat(trgSubDir, &st) != 0)
        {
            // Creates a directory with the permissions of 755 (standard Linux permission)
            if (mkdir(trgSubDir, 0755) != 0)
            {
                sprintf(message, "Failed to create directory %s", entry->d_name);
                SystemLog(message, ASLEEP);
                free(srcSubDir);
                free(trgSubDir);
                continue;
            }
            sprintf(message, "Created directory %s in target", entry->d_name);
            SystemLog(message, COPIED);
        }

        // Recursive call for next directory
        struct configStruct subConfig = *configurations;
        subConfig.sourceDir = srcSubDir;
        subConfig.targetDir = trgSubDir;
        RecursiveMatch(&subConfig);

        free(srcSubDir);
        free(trgSubDir);
    }

    // Deleting not needed directories from target
    sprintf(message, "Began to look for directories to delete in %s", configurations->sourceDir);
    SystemLog(message, ACTION);
    DIR *targetDir = opendir(configurations->targetDir); // opening targetDir
    if (targetDir)
    {
        struct dirent *targetEntry;
        while ((targetEntry = readdir(targetDir)) != NULL)
        {
            if (targetEntry->d_name[0] == '.' || targetEntry->d_type != DT_DIR)
                continue;

            char *srcCheckPath = BuildPath(configurations->sourceDir, targetEntry->d_name);
            struct stat srcCheckStat;
            bool existsInSource = (stat(srcCheckPath, &srcCheckStat) == 0 &&
                                   S_ISDIR(srcCheckStat.st_mode));
            free(srcCheckPath);

            if (!existsInSource)
            {
                char *trgDelPath = BuildPath(configurations->targetDir, targetEntry->d_name);
                if (RemoveDirectoryRecursively(trgDelPath))
                {
                    sprintf(message, "Removed directory: %s", targetEntry->d_name);
                    SystemLog(message, DELETED);
                }
                else
                {
                    sprintf(message, "Failed to remove directory: %s", targetEntry->d_name);
                    SystemLog(message, ASLEEP);
                }
                free(trgDelPath);
            }
        }
        closedir(targetDir);
    }

    // Set the target time to source
    struct utimbuf times;
    times.actime = srcDirStat.st_atime;
    times.modtime = srcDirStat.st_mtime;
    utime(configurations->targetDir, &times);

    closedir(dir);
    return true;
}

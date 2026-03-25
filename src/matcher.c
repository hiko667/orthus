#include "matcher.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
bool match(struct configStruct * configurations)
{
    DIR *dir;
    struct dirent *entry;
    dir = opendir("./");
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(dir);
    return true;
}
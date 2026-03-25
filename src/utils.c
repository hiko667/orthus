#define _DEFAULT_SOURCE // need this cause i use arch btw
#include "utils.h"								//  I
#include <stdio.h>        						//  I
#include <dirent.h>								//  I
bool is_dir(const char *path) 					//  I
{												//  I
	struct stat st;								//  [------]
	return (stat(path, &st) == 0) && S_ISDIR(st.st_mode);//I
}														 //I
int countFiles(const char * path)						// I
{													 //    I
	int count = 0;									 // FOR THIS
	struct dirent * entry;							 //    I
	DIR * dir = opendir(path);						 //    I	
	if(dir == NULL) return -1;						 //   \ /
	while ((entry = readdir(dir)) != NULL)           //    V
	{
        if (entry->d_name[0] == '.' || entry->d_type != DT_REG) continue;
        count++;
    }
	closedir(dir);
	return count;
}
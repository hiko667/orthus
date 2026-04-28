#include "configuration.h"
#include <stdio.h>
#include "utils.h"

#define VERSION 1.0

// Find the source's path, return true if it's valid and add to the configuration struct
bool SetSource(struct configStruct * configurations, const char * path)
{
	if(!IsDir(path))
	{
		printf("Invalid source directory!\n");
		return false;
	}
	if(path[0] == '.'){
		size_t sourceDirSize = strlen(path) + strlen(configurations->globalDir) + 1;
		configurations->sourceDir = malloc(sourceDirSize);
		snprintf(configurations->sourceDir, sourceDirSize, "%s%s", configurations->globalDir, path+1);
	}
	else{
		configurations->sourceDir = malloc(strlen(path) + 1);
		strcpy(configurations->sourceDir, path);
	}
	
	
	return true;
}
// Find the target's path, return true if it's valid and add to the configuration struct
bool SetTarget(struct configStruct * configurations, const char * path)
{
	if(!IsDir(path))
	{
		printf("Invalid target directory!\n");
		return false;
	}
	if(path[0] == '.'){
		size_t targetDirSize = strlen(path) + strlen(configurations->globalDir) + 1;
		configurations->targetDir = malloc(targetDirSize);
		snprintf(configurations->targetDir, targetDirSize, "%s%s", configurations->globalDir, path+1);
	}
	else{
		configurations->targetDir = malloc(strlen(path) + 1);
		strcpy(configurations->targetDir, path);
	}
	
	return true;
}
// The output of the -h flag
void GiveHelp()
{
	printf("Orthus version %.1f\n", VERSION);
	printf("Use following flags for configs: \n");
	printf("-h -- help\n");
	printf("-s -- set up source directory\n");
	printf("-t -- set up target directory\n");
	printf("-f -- set up frequency in minutes\n");
	printf("-R -- use recursion\n");
	printf("-m -- set byte limit on big files\n");
}
// Read each argument and save to the configuration one by one, return false if anything goes wrong
bool ReadArguments(struct configStruct * configurations, int argc, char * argv[])
{
	configurations->recursive = false;
	int i = 1;
	while (i < argc)
	{
		// Skip if an argument doesn't start with '-'
		if (argv[i][0] != '-')
		{
			i++;
			continue;
		}

		switch (argv[i][1])
		{
			// Set source
			case 's' :
				// check if the formatting and path are valid
				if(i + 1 >= argc || strlen(argv[i]) > 2 || !SetSource(configurations, argv[i+1]))
					return false;
				i += 2;
				break;
			
			// Set target
			case 't' :
				// check if the formatting and path are valid
				if(i + 1 >= argc || strlen(argv[i]) > 2 || !SetTarget(configurations, argv[i+1]))
					return false;
				i += 2;
				break;
			
			// Set frequency
			case 'f':
				// check if the formatting and path are valid
				if(i + 1 >= argc ||strlen(argv[i]) > 2 || atoi(argv[i+1]) < 0)
					return false;
				configurations->awakeningFrequency = atoi(argv[i+1]);
				i += 2;
				break;
			
			// Get help
			case 'h' :
				GiveHelp();
				return false;
			
			// Set recursive
			case 'R' :
				// check if the formatting is valid
				if(strlen(argv[i]) > 2) return false;
				configurations->recursive = true;
				i++;
				break;
			
			// Byte limit of small files
			case 'm':
				// check if the formatting is valid
				if(i + 1 >= argc || atoi( argv[i+1]) < 0 || strlen(argv[i]) >2) return false;
				configurations->minSizeToBeBig = atoi(argv[i+1]);
				i+=2;
				break;

			default:
				i++;
				break;
		}
	}
	return true;
}
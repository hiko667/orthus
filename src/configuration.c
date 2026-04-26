#include "configuration.h"
#include <stdio.h>
#include "utils.h"

#define VERSION 1.0

bool SetSource(struct configStruct * configurations, const char * path)
{
	if(!IsDir(path))
	{
		printf("Invalid source directory!\n");
		return false;
	}
	strcpy(configurations->sourceDir, path);
	return true;
}
bool SetTarget(struct configStruct * configurations, const char * path)
{
	if(!IsDir(path))
	{
		printf("Invalid target directory!\n");
		return false;
	}
	strcpy(configurations->targetDir, path);
	return true;
}
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
bool ReadArguments(struct configStruct * configurations, int argc, char * argv[])
{
	int i = 1;
	while (i < argc)
	{
		if (argv[i][0] != '-')
		{
			i++;
			continue;
		}
		switch (argv[i][1])
		{
			case 's' :
				if(i + 1 >= argc || strlen(argv[i]) > 2 || !SetSource(configurations, argv[i+1]))
					return false;
				i += 2; break;
			case 't' :
				if(i + 1 >= argc || strlen(argv[i]) > 2 || !SetTarget(configurations, argv[i+1]))
					return false;
				i += 2; break;
			case 'f':
				if(i + 1 >= argc ||strlen(argv[i]) > 2 || atoi(argv[i+1]) < 0) return false;
				configurations->awakeningFrequency = atoi(argv[i+1]);
				i += 2;
				break;
			case 'h' : GiveHelp(); return false;
			case 'R' :
				if(strlen(argv[i]) > 2) return false;
				configurations->recursive = true; i ++; break;
			case 'm':
				if(i + 1 >= argc || atoi( argv[i+1]) < 0 || strlen(argv[i]) >2) return false;
				configurations->minSizeToBeBig = atoi(argv[i+1]);
				i+=2; break;

			default:
				i++;
				break;
		}
	}
	return true;
}
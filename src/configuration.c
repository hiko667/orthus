#include "configuration.h"
#include <stdio.h>
#include "utils.h"

#define VERSION 1.0

bool setSource(struct configStruct * configurations, const char * path)
{
	if(!is_dir(path))
	{
		printf("Invalid source directory!\n");
		return false;
	}
	strcpy(configurations->sourceDir, path);
	return true;
}
bool setTarget(struct configStruct * configurations, const char * path)
{
	if(!is_dir(path))
	{
		printf("Invalid target directory!\n");
		return false;
	}
	strcpy(configurations->targetDir, path);
	return true;
}
void giveHelp()
{
	printf("Orthus version %.1f\n", VERSION);
	printf("Use following flags for configs: \n");
	printf("-h -- help\n");
	printf("-s -- set up source directory\n");
	printf("-t -- set up target directory\n");
	printf("-f -- set up frequency\n");
	printf("-R -- use recursion\n");
	printf("-m to set byte limit on big files\n");
}
bool readArguments(struct configStruct * configurations, int argc, char * argv[])
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
				if(i + 1 >= argc || strlen(argv[i]) > 2 || !setSource(configurations, argv[i+1]))
					return false;
				i += 2; break;
			case 't' :
				if(i + 1 >= argc || strlen(argv[i]) > 2 || !setTarget(configurations, argv[i+1]))
					return false;
				i += 2; break;
			case 'f':
				if(i + 1 >= argc ||strlen(argv[i]) > 2 || atoi(argv[i+1]) < 0) return false;
				configurations->awakeningFrequency = atoi(argv[i+1]);
				i += 2;
				break;
			case 'h' : giveHelp(); return false;
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
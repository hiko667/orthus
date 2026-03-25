#include "configuration.h"
#include <stdio.h>
#include "utils.h"

#define VERSION 1.0 

bool setSource(struct configStruct * configurations, const char * path)
{
	if(!is_dir(path)) 
	{
		printf("Invallid source directory!\n");
		return false;
	}
	strcpy(configurations->sourceDir, path);
}
bool setTarget(struct configStruct * configurations, const char * path)
{
	if(!is_dir(path)) return false;
	strcpy(configurations->targetDir, path);
}
void giveHelp()
{
	printf("Orthus version %.1f\n", VERSION);
	printf("Use following flags for configs: \n");
	printf("-h -- help\n");
	printf("-s -- set up source directory\n");
	printf("-t -- set up target directory\n");
	printf("-R -- use recursion\n");
	printf("-m to set byte limit on big files\n");
}
bool readArguments(struct configStruct * configurations, int argc, char * argv[])
{
	int i = 1;
	while (i < argc)
	{
		switch (argv[i][1])
		{
			case 's' : if(!setSource(configurations, argv[i+1]) || strlen(argv[i]) > 2) 
				return false; i += 2; break;
			case 't' : 
				if(!setTarget(configurations, argv[i+1]) || strlen(argv[i]) > 2)
					return false; 
				i += 2; break;
			case 'h' : giveHelp(); return false;
			case 'R' : 
				if(strlen(argv[i]) > 2);
					return false;
				configurations->recursive = true; i ++; break;
			case 'm':;
			default:
				break;
		}
	}
	return true;
}

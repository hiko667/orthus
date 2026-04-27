#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include "config_struct.h"
#include "configuration.h"
#include "matcher.h"
#include "logs.h"

int main(int argc, char * argv[])
{
	//ustawienie konfiguracji programu
	struct configStruct configurations = {0};
	configurations.globalDir = getcwd(NULL, 0);
	configurations.awakeningFrequency = 5;
	configurations.minSizeToBeBig = 20 * 1024 * 1024; //20 MiB
	if(!ReadArguments(&configurations, argc, argv)) return 1; //zczytanie argumentów programuS
	printf("%s\n%s\n", configurations.sourceDir, configurations.targetDir);
	printf("Configuration completed, demonizing...\n");

	if (DaemonizeProcess() != 0) {
		return -1;
	}

	while (1)
	{
		if (configurations.recursive)
		{
			RecursiveMatch(&configurations);
		}
		else
		{
			Match(&configurations);
		}
		SystemLog("Daemon goes to sleep.", ASLEEP);
		sleep(configurations.awakeningFrequency * 60);
		}
	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include "config_struct.h"
#include "configuration.h"
#include "matcher.h"

int main(int argc, char * argv[])
{
	struct configStruct configurations = {0};
	if(!readArguments(&configurations, argc, argv)) return 1;
	printf("%s\n%s\n", configurations.sourceDir, configurations.targetDir);
	printf("Configuration completed, demonizing...\n");
	if (daemonize_process() != 0) {
		return -1;
	}

	while (1) {
		match(&configurations);
		sleep(configurations.awakeningFrequency*60);
	}

    return 0;
}
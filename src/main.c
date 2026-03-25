#include <stdio.h>
#include <stdlib.h>
#include "config_struct.h"
#include "configuration.h"
#include "matcher.h"

void daemonize() {

}


int main(int argc, char * argv[])
{
	struct configStruct configurations = {0};
	if(!readArguments(&configurations, argc, argv)) return 1;
	printf("%s\n%s\n", configurations.sourceDir, configurations.targetDir);
	printf("Configuration completed, demonizing...\n");
	daemonize();
    return 0;
}
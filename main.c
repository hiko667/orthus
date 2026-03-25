#include <stdio.h>
#include <stdlib.h>
#include "config_struct.h"
#include "configuration.h"


void daemonize() {

}


int main(int argc, char * argv[])
{
	struct configStruct * configurations;
	if(!readArguments(configurations, argc, argv)) return 1;
	printf("Configuration completed, demonizing...\n");
	printf("%s", configurations->sourceDir);

    return 0;
}
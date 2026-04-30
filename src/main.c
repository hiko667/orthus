#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"
#include "config_struct.h"
#include "configuration.h"
#include "matcher.h"
#include "logs.h"
#include <signal.h>

struct configStruct con = {0};

void CatchSignalToRunMatch(int sig)
{   // after detecting signal synchronizes agin
	SystemLog("Daemon awoke by SIGUSR1. Begining to match", AWOKE);
	if (con.recursive)
	{
		RecursiveMatch(&con);
	}
	else
	{
		Match(&con);
	}
}
int main(int argc, char * argv[])
{
	con.globalDir = getcwd(NULL, 0);
	con.awakeningFrequency = 5;
	con.minSizeToBeBig = 20 * 1024 * 1024; //20 MiB
	if(!ReadArguments(&con, argc, argv)) return 1; 
	printf("%s\n%s\n", con.sourceDir, con.targetDir);
	printf("Configuration completed, demonizing...\n");
	signal(SIGUSR1, CatchSignalToRunMatch);

	if (DaemonizeProcess() != 0) return -1; 

	while (1)
	{
    // Awake the deamon
		SystemLog("Daemon awoke. Begining to match", AWOKE);
		if (con.recursive)		
			RecursiveMatch(&con);
		else
			Match(&con);
    // Go to sleep
		SystemLog("Daemon goes to sleep.", ASLEEP);
		sleep(con.awakeningFrequency * 60);
		}
	return 0;
}
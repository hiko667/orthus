#ifndef CONFIG_H
#define CONFIG_H
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "config_struct.h"

bool SetSource(struct configStruct * configurations, const char * path);
bool SetTarget(struct configStruct * configurations, const char * path);
void GiveHelp();
bool ReadArguments(struct configStruct * configurations, int argc, char * argv[]);


#endif 
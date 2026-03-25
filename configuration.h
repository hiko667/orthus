#ifndef CONFIG_H
#define CONFIG_H
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "config_struct.h"

bool setSource(struct configStruct * configurations, const char * path);
bool setTarget(struct configStruct * configurations, const char * path);
void giveHelp();
bool readArguments(struct configStruct * configurations, int argc, char * argv[]);


#endif 
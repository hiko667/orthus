#include "logs.h"
#include <time.h>
#include <syslog.h>
#include <string.h>
const char *  getLogType(char * typeOfLog, enum logType type)
{
    switch (type)
    {
    case ASLEEP: typeOfLog = "FALLING ASLEEP:"; break;
    case AWOKE: typeOfLog = "AWOKE FROM SLEEP:"; break;
    case COPIED: typeOfLog = "COPIED FILE:"; break;
    case DELETED: typeOfLog = "DELETED FILE:"; break;
    default: break;
    }
}
void addTimeToString(char * message);
{
    time_t currentTime;
    time(&currentTime);
    char * timeString = 
}
void completeMessage(char * message, char * additionalInformation, enum logType type)
{
    message[0] = '\0';
    char * typeOfLog;
    getLogType(typeOfLog, type);
}

void systemLog(char * additionalInformation, enum logType type)
{
    openlog("Orthus", LOG_PID, LOG_DAEMON);
    char message[1024];
    completeMessage(message, additionalInformation, type);
    syslog(LOG_INFO, message);
    closelog();
}

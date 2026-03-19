#include "logs.h"
#include <time.h>
#include <syslog.h>
#include <string.h>

void removeNewline(char *s) {
    s[strcspn(s, "\n")] = '\0';
}
void addLogType(char * messgage, enum logType type)
{
    switch (type)
    {
    case ASLEEP:  strcat(messgage, "FALLING ASLEEP:"); break;
    case AWOKE: strcat(messgage, "AWOKE FROM SLEEP:"); break;
    case COPIED: strcat(messgage, "COPIED FILE:"); break;
    case DELETED: strcat(messgage, "DELETED FILE:"); break;
    default: break;
    }
}
void addTime(char * message)
{
    time_t currentTime;
    time(&currentTime);
    char * timeString = ctime(&currentTime);
    removeNewline(timeString);
    strcat(message, timeString);
    strcat(message,":");
}
void completeMessage(char * message, char * additionalInformation, enum logType type)
{
    message[0] = '\0';
    addLogType(message, type);
    addTime(message);
    strcat(message, additionalInformation);
}

void systemLog(char * additionalInformation, enum logType type)
{
    openlog("Orthus", LOG_PID, LOG_DAEMON);
    char message[1024];
    completeMessage(message, additionalInformation, type);
    syslog(LOG_INFO, message);
    closelog();
}

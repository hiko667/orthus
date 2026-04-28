#include "logs.h"
#include <time.h>
#include <syslog.h>
#include <string.h>

void RemoveNewLine(char *s)
{
    s[strcspn(s, "\n")] = '\0';
}
// Add the right description to the end of the message depending on the logType
void AddLogType(char *messgage, enum logType type)
{
    switch (type)
    {
    case ASLEEP:
        strcat(messgage, "FALLING ASLEEP:");
        break;
    case AWOKE:
        strcat(messgage, "AWOKE FROM SLEEP:");
        break;
    case COPIED:
        strcat(messgage, "COPIED FILE:");
        break;
    case DELETED:
        strcat(messgage, "DELETED FILE:");
        break;
    case ACTION:
        strcat(messgage, "PERFORMED ACTION:");
    default:
        break;
    }
}
// Add system date and a ':' to the end of the message
void AddTime(char *message)
{
    time_t currentTime;
    time(&currentTime);
    char *timeString = ctime(&currentTime);
    RemoveNewLine(timeString);
    strcat(message, timeString);
    strcat(message, ":");
}
// Finish writing a message by using AddLogType and AddTime
void CompleteMessage(char *message, const char *additionalInformation, enum logType type)
{
    message[0] = '\0';
    AddLogType(message, type);
    AddTime(message);
    strcat(message, additionalInformation);
}
// Add a log to the system using CompleteMessage
void SystemLog(const char *additionalInformation, enum logType type)
{
    openlog("Orthus", LOG_PID, LOG_DAEMON);
    char message[1024];
    CompleteMessage(message, additionalInformation, type);
    syslog(LOG_INFO, "%s", message);
    closelog();
}

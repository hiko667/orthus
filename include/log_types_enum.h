#ifndef LOG_ENUM_H
#define LOG_ENUM_H

enum logType {
    ASLEEP = 1,
    AWOKE = 2,
    COPIED = 3,
    DELETED = 4,
    ACTION = 5
};
typedef enum logType logType_t;
#endif
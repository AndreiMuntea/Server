#ifndef UTILS_CONSTANTS_H
#define UTILS_CONSTANTS_H 

#include <Windows.h>

#define PIPE_PREFIX              TEXT("\\\\.\\pipe\\")
#define PIPE_DEFAULT_NAME        TEXT("hardcoded")
#define PIPE_NAME_MAX_SIZE       255
#define DEFAULT_TIME_OUT_VALUE   10000

#define REQUEST_BUFFER_SIZE      512
#define REPLY_BUFFER_SIZE        512

#define DEFAULT_LOG_FILE_NAME    TEXT("log.txt")



#endif //UTILS_CONSTANTS_H
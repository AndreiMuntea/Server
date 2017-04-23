#ifndef UTILS_CONSTANTS_H
#define UTILS_CONSTANTS_H 

#include <Windows.h>

#define PIPE_PREFIX              "\\\\.\\pipe\\"
#define PIPE_DEFAULT_NAME        "hardcoded"
#define PIPE_NAME_MAX_SIZE       255
#define DEFAULT_TIME_OUT_VALUE   300000
#define DEFAULT_SLEEP_TIME       500

#define DEFAULT_BUFFER_SIZE      512
#define REQUEST_BUFFER_SIZE      3
#define REPLY_BUFFER_SIZE        3

#define DEFAULT_LOG_FILE_NAME    "log.txt"
#define DEFAULT_USER_FILE_NAME   "clients.txt"



#endif //UTILS_CONSTANTS_H
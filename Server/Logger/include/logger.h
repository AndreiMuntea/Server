#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <Windows.h>
#include "../../Utils/include/definitions.h"


typedef struct _LOGGER
{
   LPCRITICAL_SECTION criticalSection;
   HANDLE fileHandle;

}LOGGER, *PLOGGER;


/*
* Creates a new instance of a LOGGER
* @params: logger - a PLOGGER*
* @params: logFileName - a LPCTSTR, the name of log file
*                      - if NULL, the default name will be used instead
* @returns: STATUS - EXIT_SUCCESS_STATUS if instance is created without errors
*                  - error code otherwise
*/
STATUS CreateLogger(PLOGGER* logger, LPCTSTR fileName);

/**
* Destroys an instance of a LOGGER
* @params: logger - a PLOGGER*, the instance to be destroyed
*/
void DestroyLogger(PLOGGER* logger);

#endif //LOGGER_LOGGER_H
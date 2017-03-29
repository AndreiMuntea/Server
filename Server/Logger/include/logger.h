#ifndef LOGGER_LOGGER_H
#define LOGGER_LOGGER_H

#include <Windows.h>
#include "../../Utils/include/definitions.h"


typedef struct _LOGGER
{
   LPCRITICAL_SECTION criticalSection;
   HANDLE fileHandle;

}LOGGER, *PLOGGER;

STATUS CreateLogger(PLOGGER* logger, LPCTSTR fileName);

void DestroyLogger(PLOGGER* logger);

#endif //LOGGER_LOGGER_H
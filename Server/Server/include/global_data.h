#ifndef SERVER_GLOBAL_DATA_H
#define SERVER_GLOBAL_DATA_H
#include "../../Logger/include/logger.h"


typedef struct _GLOBAL_DATA
{
   PLOGGER logger;
}GLOBAL_DATA, *PGLOBAL_DATA;

extern GLOBAL_DATA globals;

STATUS InitGlobalData(LPCTSTR logFileName);

void UnitialiseGlobalData();

#endif //SERVER_GLOBAL_DATA_H
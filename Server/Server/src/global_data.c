#include "../include/global_data.h"

GLOBAL_DATA globals;

STATUS InitGlobalData(LPCSTR logFileName)
{
   STATUS status;
   PLOGGER tempLogger;

   status = EXIT_SUCCESS_STATUS;
   tempLogger = NULL;

   status = CreateLogger(&tempLogger, logFileName);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   globals.logger = tempLogger;

EXIT:
   if(!SUCCESS(status))
   {
      DestroyLogger(&tempLogger);
      globals.logger = NULL;
   }
   return status;
}

void UnitialiseGlobalData()
{
   DestroyLogger(&globals.logger);
}
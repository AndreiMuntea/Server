#include "../include/logger.h"
#include "../../Utils/include/constants.h"


STATUS CreateLogger(PLOGGER* logger, LPCTSTR fileName)
{
   PLOGGER tempLogger;
   STATUS status;
   HANDLE fileHandle;
   LPCRITICAL_SECTION section;
   LPCTSTR fileNameTemp;

   fileNameTemp = NULL;
   status = EXIT_SUCCESS_STATUS;
   tempLogger = NULL;
   fileHandle = NULL;
   section = NULL;

   section = (LPCRITICAL_SECTION)malloc(sizeof(CRITICAL_SECTION));
   if(NULL == section)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }
   
   fileNameTemp = fileName;
   if(fileName == NULL)
   {
      fileNameTemp = DEFAULT_LOG_FILE_NAME;
   }

   tempLogger = (PLOGGER)malloc(sizeof(LOGGER));
   if(tempLogger == NULL)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }
   tempLogger->fileHandle = NULL;
   tempLogger->criticalSection = NULL;

   fileHandle = CreateFile(
      fileNameTemp,
      GENERIC_WRITE,
      0,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      NULL
   );
   if (fileHandle == INVALID_HANDLE_VALUE)
   {
      status = FILE_OPEN_ERROR;
      goto EXIT;
   }

   InitializeCriticalSection(section);

   tempLogger->criticalSection = section;
   tempLogger->fileHandle = fileHandle;

   *logger = tempLogger;

EXIT:
   if(!SUCCESS(status))
   {
      if (fileHandle != INVALID_HANDLE_VALUE)
      {
         CloseHandle(fileHandle);
      }

      free(section);
      section = NULL;

      free(tempLogger);
      tempLogger = NULL;
   }
   return status;
}

void DestroyLogger(PLOGGER* logger)
{
  if(logger == NULL)
  {
     goto EXIT;
  }

   if(*logger == NULL)
   {
      goto EXIT;
   }

   CloseHandle((*logger)->fileHandle);
   DeleteCriticalSection((*logger)->criticalSection);
   free((*logger)->criticalSection);

   free(*logger);
   *logger = NULL;

EXIT:
   return;
}
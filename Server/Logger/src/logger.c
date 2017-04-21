#include <stdarg.h>
#include <Strsafe.h>

#include "../include/logger.h"
#include "../../Helper/include/constants.h"



STATUS CreateLogger(PLOGGER* logger, LPCSTR fileName)
{
   PLOGGER tempLogger;
   STATUS status;
   HANDLE fileHandle;
   LPCRITICAL_SECTION section;
   LPCSTR fileNameTemp;

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

   fileHandle = CreateFileA(
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


STATUS Log(PLOGGER logger, int noMessages, ...)
{
   STATUS status;
   int i;
   va_list args;
   LPSTR buffer;
   size_t buffSize;
   BOOL successOperation;
   DWORD writtenBytes;

   successOperation = TRUE;
   writtenBytes = 0;
   buffSize = 0;
   status = EXIT_SUCCESS_STATUS;
   i = 0;
   buffer = NULL;

   if (NULL == logger)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   if (NULL == logger->fileHandle)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   va_start(args, noMessages);


   EnterCriticalSection(logger->criticalSection);
   for(i = 0; i < noMessages; ++i)
   {
      buffer = va_arg(args, LPSTR);
      if (StringCchLengthA(buffer, STRSAFE_MAX_CCH, &buffSize) != S_OK)
      {
         status = INVALID_PARAMETER;
         goto EXIT;
      }

      buffSize *= sizeof(CHAR);
      successOperation = WriteFile(
         logger->fileHandle,
         buffer,
         buffSize,
         &writtenBytes,
         NULL
      );

      if (!successOperation || writtenBytes != buffSize)
      {
         status = GetLastError();
         goto EXIT;
      }
   }


EXIT:
   LeaveCriticalSection(logger->criticalSection);
   va_end(args);
   return status;
}
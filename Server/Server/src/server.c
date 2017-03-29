#include "../include/server.h"
#include "../../Utils/include/strutils.h"
#include "../../Utils/include/constants.h"

STATUS InitialiseNamedPipe(LPCTSTR pipeFileName, PHANDLE pipeHandle)
{
   STATUS status;
   HANDLE handle;

   handle = NULL;
   status = EXIT_SUCCESS_STATUS;

   if(NULL == pipeHandle)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   handle = CreateNamedPipe(
      pipeFileName,
      PIPE_ACCESS_DUPLEX,
      PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
      PIPE_UNLIMITED_INSTANCES,
      REPLY_BUFFER_SIZE,
      REQUEST_BUFFER_SIZE,
      0,
      NULL);
   
   if(handle == INVALID_HANDLE_VALUE)
   {
      status = PIPE_FAILURE;
      goto EXIT;
   }

   *pipeHandle = handle;


EXIT:
   if(!SUCCESS(status))
   {
      if(handle != INVALID_HANDLE_VALUE && handle != NULL)
      {
         CloseHandle(handle);
         handle = NULL;
      }
   }

   return status;   
}

STATUS CreateServer(PSERVER* server, LPCTSTR pipeFileName, LPCTSTR logFileName)
{
   STATUS status;
   PSERVER tempServer;
   PLOGGER logger;
   LPTSTR pipeName;


   logger = NULL;
   status = EXIT_SUCCESS_STATUS;
   tempServer = NULL;
   pipeName = NULL;

   status = CreateLogger(&logger, logFileName);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   CreatePipeName(&pipeName, pipeFileName);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   tempServer = (PSERVER)malloc(sizeof(SERVER));
   if (NULL == tempServer)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempServer->logger = logger;
   tempServer->pipeName = pipeName;
   tempServer->closeFlag = FALSE;

   *server = tempServer;


EXIT:
   if(!SUCCESS(status))
   {
      DestroyLogger(&logger);
      DestroyServer(&tempServer);
   }

   return status;
}

void DestroyServer(PSERVER* server)
{
   if(NULL == server)
   {
      goto EXIT;
   }

   if(NULL == *server)
   {
      goto EXIT;
   }
   
   DestroyLogger(&(*server)->logger);
   free((*server)->pipeName);

   free(*server);
   server = NULL;

EXIT:
   return;
}
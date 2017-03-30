#include <tchar.h>

#include "../include/server.h"
#include "../../Utils/include/strutils.h"
#include "../../Utils/include/constants.h"
#include "../include/client_session.h"
#include "../include/thread_context.h"

STATUS InitialiseNamedPipe(LPCTSTR pipeFileName, PHANDLE pipeHandle)
{
   STATUS status;
   HANDLE handle;

   handle = INVALID_HANDLE_VALUE;
   status = EXIT_SUCCESS_STATUS;

   if (NULL == pipeHandle)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   handle = CreateNamedPipe(
      pipeFileName,
      PIPE_ACCESS_DUPLEX,
      PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
      PIPE_UNLIMITED_INSTANCES,
      REPLY_BUFFER_SIZE,
      REQUEST_BUFFER_SIZE,
      0,
      NULL);

   if (handle == INVALID_HANDLE_VALUE)
   {
      status = PIPE_FAILURE;
      goto EXIT;
   }

   *pipeHandle = handle;


EXIT:
   if (!SUCCESS(status))
   {
      if (handle != INVALID_HANDLE_VALUE && handle != NULL)
      {
         CloseHandle(handle);
         handle = NULL;
      }
   }

   return status;
}

PSERVER Reference(PSERVER server)
{
   server->refCounter++;
   return server;
}

STATUS CreateServer(PSERVER* server, LPCTSTR pipeFileName)
{
   STATUS status;
   PSERVER tempServer;
   LPTSTR pipeName;

   status = EXIT_SUCCESS_STATUS;
   tempServer = NULL;
   pipeName = NULL;

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

   tempServer->pipeName = pipeName;
   tempServer->closeFlag = FALSE;
   tempServer->refCounter = 1;

   *server = tempServer;


EXIT:
   if (!SUCCESS(status))
   {
      DestroyServer(&tempServer);
   }

   return status;
}

void DestroyServer(PSERVER* server)
{
   if (NULL == server)
   {
      goto EXIT;
   }

   if (NULL == *server)
   {
      goto EXIT;
   }

   (*server)->refCounter--;
   if ((*server)->refCounter != 0)
   {
      goto EXIT;
   }

   free((*server)->pipeName);
   free(*server);
   *server = NULL;

EXIT:
   return;
}

void SetStopFlag(PSERVER server)
{
   server->closeFlag = TRUE;
}

STATUS Run(PSERVER server)
{
   BOOL clientConnected;
   HANDLE pipeHandle;
   PCLIENT_SESSION clientSession;
   PTHREAD_CONTEXT threadContext;
   STATUS status;

   threadContext = NULL;
   clientSession = NULL;
   clientConnected = FALSE;
   pipeHandle = INVALID_HANDLE_VALUE;
   status = EXIT_SUCCESS_STATUS;

   for (;;)
   {
      //TO DO ADD LOG
      if(server->closeFlag)
      {
         break;
      }

      //TO DO ADD LOG
      threadContext = NULL;
      clientSession = NULL;

      status = InitialiseNamedPipe(server->pipeName, &pipeHandle);

      //TO DO ADD LOG
      if (!SUCCESS(status))
      {
         goto EXIT;
      }

      clientConnected = ConnectNamedPipe(pipeHandle, NULL);

      //TO DO ADD LOG
      if (clientConnected)
      {
         //TO DO ADD LOG
         status = CreateClientSession(&clientSession, pipeHandle);
         if (!SUCCESS(status))
         {
            goto EXIT;
         }

         //TO DO ADD LOG
         status = CreateThreadContext(&threadContext, Reference(server), clientSession);
         if (!SUCCESS(status))
         {
            goto EXIT;
         }

         //TO DO ADD LOG
         ServeClient(threadContext);
      }
      else
      {
         //TO DO ADD LOG
         CloseHandle(pipeHandle);
      }
   }

EXIT:
   if (!SUCCESS(status))
   {
      TerminateClientSession(&clientSession);
      DestroyThreadContext(&threadContext);
      SetStopFlag(server);
   }
   return status;
}

DWORD WINAPI ServeClient(LPVOID lpvParam)
{
   PTHREAD_CONTEXT threadContext;
   HANDLE heapHandle;
   TCHAR* requestBuffer;
   TCHAR* replyBuffer;
   STATUS status;
   DWORD readBytes;
   DWORD writtenBytes;
   DWORD replyBytes;
   HANDLE pipeHandle;
   BOOL successOperation;

   successOperation = FALSE;
   pipeHandle = INVALID_HANDLE_VALUE;
   readBytes = 0;
   writtenBytes = 0;
   replyBytes = 0;
   status = EXIT_SUCCESS_STATUS;
   threadContext = NULL;
   heapHandle = GetProcessHeap();
   requestBuffer = NULL;
   replyBuffer = NULL;

   //TO DO ADD LOG
   if (NULL == lpvParam)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   threadContext = (PTHREAD_CONTEXT)lpvParam;
   pipeHandle = threadContext->clientSession->pipeHandle;

   //TO DO ADD LOG
   requestBuffer = (TCHAR*)HeapAlloc(heapHandle, 0, REQUEST_BUFFER_SIZE * sizeof(TCHAR));
   if (NULL == requestBuffer)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   //TO DO ADD LOG
   replyBuffer = (TCHAR*)HeapAlloc(heapHandle, 0, REPLY_BUFFER_SIZE * sizeof(TCHAR));
   if (NULL == replyBuffer)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   //TO DO ADD LOG SUCCESS


   successOperation = ReadFile(
      pipeHandle,
      requestBuffer,
      REQUEST_BUFFER_SIZE * sizeof(TCHAR),
      &readBytes,
      NULL);


   if(!successOperation || readBytes == 0)
   {
      //TO DO ADD LOG
      if(GetLastError() == ERROR_BROKEN_PIPE)
      {
         status = ERROR_BROKEN_PIPE;
         goto EXIT;
      }
      else
      {
         status = GetLastError();
         goto EXIT;
      }
   }
   

   //DELETE ME
   _tprintf(TEXT("Am primit: %s\n"), requestBuffer);
   successOperation = WriteFile(
      pipeHandle,
      requestBuffer,
      readBytes,
      &writtenBytes,
      NULL
   );

   if(!successOperation ||  writtenBytes != readBytes)
   {
      status = GetLastError();
      goto EXIT;
   }

   
   FlushFileBuffers(pipeHandle);
   DisconnectNamedPipe(pipeHandle);


EXIT:
   if (NULL != requestBuffer)
   {
      HeapFree(heapHandle, 0, requestBuffer);
   }
   if (NULL != replyBuffer)
   {
      HeapFree(heapHandle, 0, replyBuffer);
   }
   CloseHandle(heapHandle);
   DestroyThreadContext(&threadContext);

   //TO DO ADD LOG

   return (DWORD)status;
}

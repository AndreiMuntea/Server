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
      DEFAULT_TIME_OUT_VALUE,
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
      if(server->closeFlag)
      {
         _tprintf(TEXT("Encountered stop flag. Stopping!\n"));
         break;
      }
      threadContext = NULL;
      clientSession = NULL;

      status = InitialiseNamedPipe(server->pipeName, &pipeHandle);
      if (!SUCCESS(status))
      {
         _tprintf(TEXT("Couldn't initialise pipe on main thread. Aborting execution!\n"));
         goto EXIT;
      }

      _tprintf(TEXT("Pipe created on main thread. Awaiting clients...\n"));
      clientConnected = ConnectNamedPipe(pipeHandle, NULL);

      
      if (clientConnected)
      {
         _tprintf(TEXT("New client connected. Beginning new session.\n"));
         status = CreateClientSession(&clientSession, pipeHandle);
         if (!SUCCESS(status))
         {
            _tprintf(TEXT("Couldn't begin a new session for new client. Aborting!\n"));
            goto EXIT;
         }

         status = CreateThreadContext(&threadContext, Reference(server), clientSession);
         if (!SUCCESS(status))
         {
            _tprintf(TEXT("Couldn't create a thread context for new client. Aborting!\n"));
            goto EXIT;
         }

         _tprintf(TEXT("Initiating new session with client...\n"));
         ServeClient((LPVOID)threadContext);
      }
      else
      {
         _tprintf(TEXT("No client connected!\n"));
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
   requestBuffer = NULL;
   replyBuffer = NULL;

   if (NULL == lpvParam)
   {
      _tprintf(TEXT("Null parameter received. Aborting execution!\n"));
      status = NULL_POINTER;
      goto EXIT;
   }

   threadContext = (PTHREAD_CONTEXT)lpvParam;
   pipeHandle = threadContext->clientSession->pipeHandle;

   //TO DO ADD LOG
   requestBuffer = (TCHAR*)malloc(REQUEST_BUFFER_SIZE * sizeof(TCHAR));
   if (NULL == requestBuffer)
   {
      _tprintf(TEXT("Not enough memory for request buffer. Aborting execution!\n"));
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   //TO DO ADD LOG
   replyBuffer = (TCHAR*)malloc(REPLY_BUFFER_SIZE * sizeof(TCHAR));
   if (NULL == replyBuffer)
   {
      _tprintf(TEXT("Not enough memory for reply buffer. Aborting execution!\n"));
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   //TO DO ADD LOG SUCCESS
   _tprintf(TEXT("Successfuly allocated all resources.\n"));

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
         _tprintf(TEXT("Client disconnected.\n"));
         status = ERROR_BROKEN_PIPE;
         goto EXIT;
      }
      else
      {
         _tprintf(TEXT("Read file failed. Aborting execution.\n"));
         status = GetLastError();
         goto EXIT;
      }
   }

   _tprintf(TEXT("Sucessfuly received message: %s.\n"), requestBuffer);
   successOperation = WriteFile(
      pipeHandle,
      requestBuffer,
      readBytes,
      &writtenBytes,
      NULL
   );

   if(!successOperation ||  writtenBytes != readBytes)
   {
      _tprintf(TEXT("Write file failed. Aborting execution.\n"));
      status = GetLastError();
      goto EXIT;
   }

   
   FlushFileBuffers(pipeHandle);
   DisconnectNamedPipe(pipeHandle);


   //DELETE ME!!!!!!!!!!!!!!!!!!!!
   SetStopFlag(threadContext->server);


EXIT:
   free(requestBuffer);
   free(replyBuffer);
   DestroyThreadContext(&threadContext);

   _tprintf(TEXT("Finishing thread execution.\n"));
   return (DWORD)status;
}

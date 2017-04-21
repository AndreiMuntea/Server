#include "../include/server.h"
#include "../../Helper/include/strutils.h"
#include "../../Helper/include/constants.h"
#include "../include/client_session.h"
#include "../include/thread_context.h"
#include "../include/global_data.h"
#include "../../User/include/user.h"
#include <stdio.h>

STATUS InitialiseNamedPipe(LPCSTR pipeFileName, PHANDLE pipeHandle)
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

   handle = CreateNamedPipeA(
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

STATUS LoadClients(PLIST* list, LPCSTR fileName)
{
   STATUS status;
   PLIST tempList;
   HANDLE fileHandle;
   PUSER tempUser;
   LPSTR buffer;
   LPSTR userNameBuffer;
   LPSTR passwordBuffer;
   DWORD idx;
   DWORD bufferSize;
   DWORD i;
   DWORD bytesRead;
   BOOL parseUserName;

   parseUserName = TRUE;
   i = 0;
   bufferSize = 0;
   idx = 0;
   bytesRead = 0;
   buffer = NULL;
   userNameBuffer = NULL;
   passwordBuffer = NULL;
   tempUser = NULL;
   fileHandle = INVALID_HANDLE_VALUE;
   status = EXIT_SUCCESS_STATUS;
   tempList = NULL;

   if (NULL == list)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   status = CreateList(&tempList, DestroyUser);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   fileHandle = CreateFileA(
      fileName,
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL
   );
   if (INVALID_HANDLE_VALUE == fileHandle)
   {
      status = GetLastError();
      goto EXIT;
   }

   buffer = (LPSTR)malloc((DEFAULT_BUFFER_SIZE + 1) * sizeof(CHAR));
   if (buffer == NULL)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   userNameBuffer = (LPSTR)malloc((DEFAULT_BUFFER_SIZE + 1) * sizeof(CHAR));
   if (userNameBuffer == NULL)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   passwordBuffer = (LPSTR)malloc((DEFAULT_BUFFER_SIZE + 1) * sizeof(CHAR));
   if (passwordBuffer == NULL)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   while (1)
   {
      if (!ReadFile(fileHandle, buffer, DEFAULT_BUFFER_SIZE, &bytesRead, NULL))
      {
         status = FILE_IO_ERROR;
         goto EXIT;
      }
      if (bytesRead == 0)
      {
         break;
      }
      bufferSize = bytesRead / sizeof(CHAR);
      for (i = 0; i < bufferSize; ++i)
      {
         if (buffer[i] == (CHAR)'\n')
         {
            continue;
         }
         if (parseUserName)
         {
            if (idx == DEFAULT_BUFFER_SIZE)
            {
               status = NOT_ENOUGH_MEMORY;
               goto EXIT;
            }
            if (buffer[i] == (CHAR)('\r'))
            {
               parseUserName = FALSE;
               userNameBuffer[idx] = '\0';
               idx = 0;
            }
            else
            {
               userNameBuffer[idx] = buffer[i];
               ++idx;
            }
         }
         else
         {
            if (idx == DEFAULT_BUFFER_SIZE)
            {
               status = NOT_ENOUGH_MEMORY;
               goto EXIT;
            }
            if (buffer[i] == (CHAR)('\r'))
            {
               parseUserName = TRUE;
               passwordBuffer[idx] = '\0';
               status = CreateUser(&tempUser, userNameBuffer, passwordBuffer);
               if (!SUCCESS(status))
               {
                  goto EXIT;
               }
               status = AddElement(tempList, tempUser);
               if (!SUCCESS(status))
               {
                  goto EXIT;
               }
               idx = 0;
            }
            else
            {
               passwordBuffer[idx] = buffer[i];
               ++idx;
            }
         }
      }
   }

   *list = tempList;

EXIT:
   if (fileHandle != INVALID_HANDLE_VALUE)
   {
      CloseHandle(fileHandle);
   }
   if (!SUCCESS(status))
   {
      DestroyUser(&tempUser);
      DestroyList(&tempList);
   }
   free(buffer);
   free(userNameBuffer);
   free(passwordBuffer);
   return status;
}

STATUS CreateServer(PSERVER* server, LPCSTR pipeFileName, LPCSTR usersFileName)
{
   STATUS status;
   PSERVER tempServer;
   LPSTR pipeName;
   PLIST tempList;

   tempList = NULL;
   status = EXIT_SUCCESS_STATUS;
   tempServer = NULL;
   pipeName = NULL;

   CreatePipeName(&pipeName, pipeFileName);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = LoadClients(&tempList, usersFileName);
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
   tempServer->users = tempList;

   *server = tempServer;


EXIT:
   if (!SUCCESS(status))
   {
      DestroyList(&tempList);
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
   DestroyList(&(*server)->users);

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
      server->closeFlag = TRUE;
      if (server->closeFlag)
      {
         Log(globals.logger, 1, TEXT("[INFO] Encountered stop flag. Stopping!\n"));
         break;
      }
      threadContext = NULL;
      clientSession = NULL;

      status = InitialiseNamedPipe(server->pipeName, &pipeHandle);
      if (!SUCCESS(status))
      {
         printf("[INFO] Couldn't initialise pipe on main thread. Aborting execution!\n");
         Log(globals.logger, 1, "[INFO] Couldn't initialise pipe on main thread. Aborting execution!\n");
         goto EXIT;
      }
      printf("[INFO] Pipe created on main thread. Awaiting clients...\n");
      Log(globals.logger, 1, "[INFO] Pipe created on main thread. Awaiting clients...\n");
      clientConnected = ConnectNamedPipe(pipeHandle, NULL);

      if (clientConnected)
      {
         printf("[INFO] New client connected. Beginning new session.\n");
         Log(globals.logger, 1, "[INFO] New client connected. Beginning new session.\n");
         status = CreateClientSession(&clientSession, pipeHandle);
         if (!SUCCESS(status))
         {
            printf("[INFO] Couldn't begin a new session for new client. Aborting!\n");
            Log(globals.logger, 1, "[INFO] Couldn't begin a new session for new client. Aborting!\n");
            goto EXIT;
         }

         status = CreateThreadContext(&threadContext, Reference(server), clientSession);
         if (!SUCCESS(status))
         {
            printf("[INFO] Couldn't create a thread context for new client. Aborting!\n");
            Log(globals.logger, 1, "[INFO] Couldn't create a thread context for new client. Aborting!\n");
            goto EXIT;
         }

         printf("[INFO] Initiating new session with client...\n");
         Log(globals.logger, 1, "[INFO] Initiating new session with client...\n");
         ServeClient((LPVOID)threadContext);
      }
      else
      {
         printf("[INFO] No client connected!\n");
         Log(globals.logger, 1, "[INFO] No client connected!\n");
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
   CHAR* requestBuffer;
   CHAR* replyBuffer;
   CHAR* statusBuffer;
   STATUS status;
   DWORD readBytes;
   DWORD writtenBytes;
   DWORD replyBytes;
   DWORD sleepTime;
   DWORD totalAvailableBytes;
   HANDLE pipeHandle;
   BOOL successOperation;
   BOOL stop;

   stop = FALSE;
   successOperation = FALSE;
   pipeHandle = INVALID_HANDLE_VALUE;
   readBytes = 0;
   writtenBytes = 0;
   replyBytes = 0;
   sleepTime = 0;
   totalAvailableBytes = 0;
   status = EXIT_SUCCESS_STATUS;
   threadContext = NULL;
   requestBuffer = NULL;
   replyBuffer = NULL;
   statusBuffer = NULL;

   if (NULL == lpvParam)
   {
      printf("[INFO] Null parameter received. Aborting execution!\n");
      Log(globals.logger, 1, "[INFO] Null parameter received. Aborting execution!\n");
      status = NULL_POINTER;
      goto EXIT;
   }

   threadContext = (PTHREAD_CONTEXT)lpvParam;
   pipeHandle = threadContext->clientSession->pipeHandle;

   //TO DO ADD LOG
   requestBuffer = (CHAR*)malloc((1 + REQUEST_BUFFER_SIZE) * sizeof(CHAR));
   if (NULL == requestBuffer)
   {
      printf("[INFO] Not enough memory for request buffer. Aborting execution!\n");
      Log(globals.logger, 1, "[INFO] Not enough memory for request buffer. Aborting execution!\n");
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   //TO DO ADD LOG
   replyBuffer = (CHAR*)malloc((1 + REPLY_BUFFER_SIZE) * sizeof(CHAR));
   if (NULL == replyBuffer)
   {
      printf("[INFO] Not enough memory for reply buffer. Aborting execution!\n");
      Log(globals.logger, 1, "[INFO] Not enough memory for reply buffer. Aborting execution!\n");
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   printf("[INFO] Successfuly allocated all resources.\n");
   Log(globals.logger, 1, "[INFO] Successfuly allocated all resources.\n");

   while (!stop)
   {
      //Check if we have data available on pipe
      successOperation = PeekNamedPipe(
         pipeHandle,
         NULL,
         0,
         NULL,
         &totalAvailableBytes,
         NULL);

      //We couldn't peek the pipe
      if (!successOperation)
      {
         status = GetLastError();
         goto EXIT;
      }

      //There is no data on pipe
      if (totalAvailableBytes == 0)
      {
         //Check if we can't wait anymore for this client 
         if (sleepTime >= DEFAULT_TIME_OUT_VALUE)
         {
            status = TIMEOUT_FAILURE;
            goto EXIT;
         }
         //We can
         else
         {
            printf("[INFO] No data available. Sleeping!\n");
            Log(globals.logger, 1, "[INFO] No data available. Sleeping!\n");
            sleepTime += DEFAULT_SLEEP_TIME;
            Sleep(DEFAULT_SLEEP_TIME);
            continue;
         }
      }

      // WE GOT DATA
      // reset counter for timeout
      sleepTime = 0;

      //Read data
      successOperation = ReadFile(
         pipeHandle,
         requestBuffer,
         REQUEST_BUFFER_SIZE * sizeof(CHAR),
         &readBytes,
         NULL);

      if (GetLastError() == ERROR_MORE_DATA)
      {
         successOperation = TRUE;
      }


      if (!successOperation || readBytes == 0)
      {
         //TO DO ADD LOG
         if (GetLastError() == ERROR_BROKEN_PIPE)
         {
            printf("[INFO] Client disconnected.\n");
            Log(globals.logger, 1, "[INFO] Client disconnected.\n");
            status = ERROR_BROKEN_PIPE;
            goto EXIT;
         }
         else
         {
            printf("[INFO] Read file failed. Aborting execution");
            Log(globals.logger, 1, "[INFO] Read file failed. Aborting execution");
            status = GetLastError();
            goto EXIT;
         }
      }

      requestBuffer[readBytes / sizeof(CHAR)] = '\0';

      printf("[INFO] Sucessfuly received message: %s\n", requestBuffer);
      Log(globals.logger, 3, "[INFO] Sucessfuly received message: ", requestBuffer,"\n");
      replyBytes = readBytes / sizeof(CHAR);
      successOperation = WriteFile(
         pipeHandle,
         requestBuffer,
         replyBytes,
         &writtenBytes,
         NULL
      );

      if (!successOperation || writtenBytes != replyBytes)
      {
         printf("[INFO] Write file failed. Aborting execution.\n");
         Log(globals.logger, 1, "[INFO] Write file failed. Aborting execution.\n");
         status = GetLastError();
         goto EXIT;
      }
   }


   FlushFileBuffers(pipeHandle);


   //DELETE ME!!!!!!!!!!!!!!!!!!!!
   SetStopFlag(threadContext->server);


EXIT:
   DisconnectNamedPipe(pipeHandle);
   free(requestBuffer);
   free(replyBuffer);
   DestroyThreadContext(&threadContext);

   statusBuffer = GetErrorMessage(status);
   Log(globals.logger, 3, "[INFO] Finishing thread execution. Exit code: ", statusBuffer, "\n");
   free(statusBuffer);

   return (DWORD)status;
}

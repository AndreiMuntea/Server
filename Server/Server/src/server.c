#include <stdio.h>
#include <Strsafe.h>

#include "../include/server.h"
#include "../../Helper/include/strutils.h"
#include "../../Helper/include/constants.h"
#include "../include/client_session.h"
#include "../include/thread_context.h"
#include "../include/global_data.h"
#include "../../User/include/user.h"
#include "../../Utils/include/thread_pool.h"
#include "../../Protocols/include/protocols.h"


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
      MAX_BUFFER_SIZE, //response buffer
      MAX_BUFFER_SIZE, //request buffer
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

STATUS CreateServer(PSERVER* server, LPCSTR pipeFileName, LPCSTR usersFileName, DWORD maxIOThreadsNumber)
{
   STATUS status;
   PSERVER tempServer;
   LPSTR pipeName;
   PLIST tempListUsers;
   PCLIENT_SESSION* tempLoggedClients;
   HANDLE* tempHIOThreadsArray;
   HANDLE runningThread;
   DWORD* tempDWIOThreadsArray;
   DWORD i;

   i = 0;
   runningThread = NULL;
   tempListUsers = NULL;
   status = EXIT_SUCCESS_STATUS;
   tempServer = NULL;
   pipeName = NULL;
   tempLoggedClients = NULL;
   tempHIOThreadsArray = NULL;
   tempDWIOThreadsArray = NULL;

   CreatePipeName(&pipeName, pipeFileName);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = LoadClients(&tempListUsers, usersFileName);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   tempLoggedClients = (PCLIENT_SESSION*)malloc(maxIOThreadsNumber * sizeof(PCLIENT_SESSION));
   if (NULL == tempLoggedClients)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempHIOThreadsArray = (HANDLE*)malloc(maxIOThreadsNumber * sizeof(HANDLE));
   if (NULL == tempHIOThreadsArray)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempDWIOThreadsArray = (DWORD*)malloc(maxIOThreadsNumber * sizeof(DWORD));
   if (NULL == tempDWIOThreadsArray)
   {
      status = BAD_ALLOCATION;
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
   tempServer->users = tempListUsers;
   tempServer->noMaxIOThreads = maxIOThreadsNumber;
   tempServer->servedClients = 0;
   tempServer->pendingPipe = INVALID_HANDLE_VALUE;
   tempServer->dwIOThreadsArray = tempDWIOThreadsArray;
   tempServer->hIOThreadsArray = tempHIOThreadsArray;
   tempServer->loggedClients = tempLoggedClients;
   tempServer->activeThreads = 0;
   tempServer->runningThread = runningThread;

   for (i = 0; i < maxIOThreadsNumber; ++i)
   {
      tempDWIOThreadsArray[i] = 0;
      tempHIOThreadsArray[i] = NULL;
      tempLoggedClients[i] = NULL;
   }

   *server = tempServer;


EXIT:
   if (!SUCCESS(status))
   {
      free(tempDWIOThreadsArray);
      free(tempHIOThreadsArray);
      free(tempLoggedClients);
      DestroyList(&tempListUsers);
      DestroyServer(&tempServer);
   }

   return status;
}

void DestroyServer(PSERVER* server)
{
   UINT32 i;

   i = 0;

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

   for (i = 0; i < (*server)->noMaxIOThreads; ++i)
   {
      if (NULL != (*server)->hIOThreadsArray[i])
      {
         WaitForSingleObject((*server)->hIOThreadsArray[i], INFINITE);
         (*server)->hIOThreadsArray[i] = NULL;
         TerminateClientSession(&(*server)->loggedClients[i]);
      }
   }

   free((*server)->dwIOThreadsArray);
   free((*server)->hIOThreadsArray);
   free((*server)->loggedClients);

   if ((*server)->pendingPipe != INVALID_HANDLE_VALUE)
   {
      CloseHandle((*server)->pendingPipe);
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
   CancelSynchronousIo(server->runningThread);
   CloseHandle(server->pendingPipe);
   server->pendingPipe = INVALID_HANDLE_VALUE;
}

void DisposeClient(PSERVER server, UINT32 index)
{
   server->activeThreads--;

   CloseHandle(server->hIOThreadsArray[index]);
   server->hIOThreadsArray[index] = NULL;

   TerminateClientSession(&server->loggedClients[index]);
   server->loggedClients[index] = NULL;
}

DWORD WINAPI Run(LPVOID argument)
{
   PSERVER server;
   BOOL clientConnected;
   HANDLE pipeHandle;
   PCLIENT_SESSION clientSession;
   PTHREAD_CONTEXT threadContext;
   STATUS status;
   UINT32 next;

   next = 0;
   threadContext = NULL;
   clientSession = NULL;
   clientConnected = FALSE;
   pipeHandle = INVALID_HANDLE_VALUE;
   status = EXIT_SUCCESS_STATUS;
   server = (PSERVER)argument;

   for (;;)
   {
      if (server->closeFlag)
      {
         Log(globals.logger, 1, TEXT("[INFO] Encountered stop flag. Stopping!\n"));
         break;
      }
      threadContext = NULL;
      clientSession = NULL;

      if (server->activeThreads == server->noMaxIOThreads)
      {
         do
         {
            next = WaitForMultipleObjects(server->noMaxIOThreads, server->hIOThreadsArray, FALSE, DEFAULT_THREAD_SLEEP);
            if (next >= WAIT_OBJECT_0 && next < WAIT_OBJECT_0 + server->noMaxIOThreads)
            {
               break;
            }
         }
         while (next == WAIT_TIMEOUT);
         if (next >= WAIT_OBJECT_0 && next < WAIT_OBJECT_0 + server->noMaxIOThreads)
         {
            DisposeClient(server, next);
         }
         else
         {
            status = next;
            goto EXIT;
         }
      }
      else
      {
         next = server->activeThreads;
      }

      status = InitialiseNamedPipe(server->pipeName, &pipeHandle);
      if (!SUCCESS(status))
      {
         printf("[INFO] Couldn't initialise pipe on main thread. Aborting execution!\n");
         Log(globals.logger, 1, "[INFO] Couldn't initialise pipe on main thread. Aborting execution!\n");
         goto EXIT;
      }
      printf("[INFO] Pipe created on main thread. Awaiting clients...\n");
      Log(globals.logger, 1, "[INFO] Pipe created on main thread. Awaiting clients...\n");

      server->pendingPipe = pipeHandle;
      clientConnected = ConnectNamedPipe(pipeHandle, NULL);

      if (clientConnected)
      {
         server->pendingPipe = INVALID_HANDLE_VALUE;
         printf("[INFO] New client connected. Beginning new session.\n");
         Log(globals.logger, 1, "[INFO] New client connected. Beginning new session.\n");
         status = CreateClientSession(&clientSession, pipeHandle, server->servedClients++);
         if (!SUCCESS(status))
         {
            printf("[INFO] Couldn't begin a new session for new client. Aborting!\n");
            Log(globals.logger, 1, "[INFO] Couldn't begin a new session for new client. Aborting!\n");
            goto EXIT;
         }

         status = CreateThreadContext(&threadContext, server, clientSession);
         if (!SUCCESS(status))
         {
            printf("[INFO] Couldn't create a thread context for new client. Aborting!\n");
            Log(globals.logger, 1, "[INFO] Couldn't create a thread context for new client. Aborting!\n");
            goto EXIT;
         }

         server->loggedClients[next] = clientSession;
         server->activeThreads++;


         printf("[INFO] Initiating new session with client...\n");
         Log(globals.logger, 1, "[INFO] Initiating new session with client...\n");
         server->hIOThreadsArray[next] = CreateThread(NULL, 0, ServeClient, threadContext, 0, &server->dwIOThreadsArray[next]);

         if (server->hIOThreadsArray[next] == NULL)
         {
            printf("[INFO] Thread Create failed.\n");
            Log(globals.logger, 1, "[INFO] Thread Create failed.\n");
            server->activeThreads--;
            goto EXIT;
         }
      }
      else
      {
         printf("[INFO] No client connected!\n");
         Log(globals.logger, 1, "[INFO] No client connected!\n");
         if (server->pendingPipe != INVALID_HANDLE_VALUE)
         {
            CloseHandle(server->pendingPipe);
            server->pendingPipe = INVALID_HANDLE_VALUE;
         }
      }
   }

EXIT:
   if (!SUCCESS(status))
   {
      TerminateClientSession(&clientSession);
      DestroyThreadContext(&threadContext);
      server->closeFlag = TRUE;
   }
   return status;
}

STATUS LogIn(PSERVER server, LPSTR userName, LPSTR password)
{
   STATUS status;
   BOOL userExists;
   PUSER tempUser;
   DWORD i;

   i = 0;
   tempUser = NULL;
   userExists = FALSE;
   status = EXIT_SUCCESS_STATUS;

   status = CreateUser(&tempUser, userName, password);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   userExists = ExistsElement(server->users, tempUser, EqualUsers);
   if (!userExists)
   {
      status = INVALID_USER_OR_PASSWORD;
      goto EXIT;
   }

   userExists = FALSE;
   for (i = 0; i < server->noMaxIOThreads && !userExists; ++i)
   {
      if (server->loggedClients[i] != NULL && server->loggedClients[i]->userName != NULL)
      {
         userExists = strcmp(server->loggedClients[i]->userName, userName) == 0 && !server->loggedClients[i]->finished;
      }
   }

   if (userExists)
   {
      status = USER_ALREADY_LOGGED_IN;
      goto EXIT;
   }

EXIT:
   DestroyUser(&tempUser);
   return status;
}

STATUS LogOut(PSERVER server, LPSTR userName)
{
   STATUS status;
   BOOL userLoggedIn;
   DWORD i;

   i = 0;
   status = EXIT_SUCCESS_STATUS;
   userLoggedIn = FALSE;

   for (i = 0; i < server->noMaxIOThreads && !userLoggedIn; ++i)
   {
      if (server->loggedClients[i] != NULL && server->loggedClients[i]->userName != NULL)
      {
         userLoggedIn = strcmp(server->loggedClients[i]->userName, userName) == 0 && !server->loggedClients[i]->finished;
         if (userLoggedIn)
         {
            server->loggedClients[i]->finished = TRUE;
         }
      }
   }

   if (!userLoggedIn)
   {
      status = USER_NOT_LOGGED_IN;
   }

   return status;
}

void Encrypt(LPSTR encryptStr, DWORD encryptSize, LPSTR key, DWORD keySize)
{
   DWORD i;

   for (i = 0; i < encryptSize; ++i)
   {
      encryptStr[i] ^= key[i % keySize];
   }
}

STATUS HandleLogoutRequest(PSERVER server, PMESSAGE request, PMESSAGE* response)
{
   STATUS status;
   PMESSAGE tempResponse;
   LPSTR respBuffer;
   BOOL loggedUser;

   loggedUser = FALSE;
   respBuffer = NULL;
   tempResponse = NULL;
   status = EXIT_SUCCESS_STATUS;

   Log(globals.logger, 3, "[INFO] Received logout request for user ", request->messageBuffer, ".\n");

   if(NULL == response)
   {
      Log(globals.logger, 1, "[WARNING] Null parameter received. Aborting logout request execution!\n");
      status = NULL_POINTER;
      goto EXIT;
   }

   status = CreateMessage(&tempResponse);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   status = LogOut(server, request->messageBuffer);
   if (!SUCCESS(status))
   {
      tempResponse->messageType = FAILURE_RESPONSE;
   }
   else
   {
      tempResponse->messageType = OK_RESPONSE;
   }
   respBuffer = GetErrorMessage(status);
   
   status = AddBuffer(tempResponse, respBuffer, TRUE);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   *response = tempResponse;

EXIT:
   if (!SUCCESS(status))
   {
      DestroyMessage(&tempResponse);
   }
   free(respBuffer);
   respBuffer = NULL;

   respBuffer = GetErrorMessage(status);
   Log(globals.logger, 5, "[INFO] Logout response for user ", request->messageBuffer, " finished with exitcode:", respBuffer, "\n");
   free(respBuffer);
   respBuffer = NULL;

   return status;
}

STATUS HandleLoginRequest(PSERVER server, PCLIENT_SESSION clientSession, PMESSAGE request, PMESSAGE* response)
{
   STATUS status;
   PMESSAGE tempResponse;
   LPSTR respBuffer;
   DWORD noBuffers, idx, username, password, key;

   noBuffers = 0;
   idx = 0;
   tempResponse = NULL;
   username = 0;
   password = 0;
   key = 0;
   status = EXIT_SUCCESS_STATUS;
   respBuffer = NULL;

   Log(globals.logger, 3, "[INFO] Received login request for user ", request->messageBuffer, ".\n");

   if (NULL == response)
   {
      Log(globals.logger, 1, "[WARNING] Null parameter received. Aborting login request execution!\n");
      status = NULL_POINTER;
      goto EXIT;
   }

   status = CreateMessage(&tempResponse);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }


   if (clientSession->userName != NULL)
   {
      tempResponse->messageType = FAILURE_RESPONSE;
      respBuffer = GetErrorMessage(SESSION_ALREADY_HAS_USER);
      status = AddBuffer(tempResponse, respBuffer, TRUE);
      goto EXIT;
   }

   for (idx = 0; idx < request->messageLength; ++idx)
   {
      if (request->messageBuffer[idx] == '\0')
      {
         noBuffers++;
         if (noBuffers == 1) password = idx + 2;
         if (noBuffers == 2) key = idx + 2;
      }
   }
   //username, pass, key
   if (noBuffers != 3)
   {
      tempResponse->messageType = FAILURE_RESPONSE;
      respBuffer = GetErrorMessage(INVALID_NO_BUFFERS);
      status = AddBuffer(tempResponse, respBuffer, TRUE);
      goto EXIT;
   }

   status = LogIn(server, request->messageBuffer + username, request->messageBuffer + password);
   respBuffer = GetErrorMessage(status);
   AddBuffer(tempResponse, respBuffer, TRUE);
   if (!SUCCESS(status))
   {
      tempResponse->messageType = FAILURE_RESPONSE;
      if (!INTERNAL_ERROR(status))
      {
         status = EXIT_SUCCESS_STATUS;
      }
   }
   else
   {
      tempResponse->messageType = OK_RESPONSE;
      status = AssignUserName(clientSession, request->messageBuffer + username);
      if (!SUCCESS(status))
      {
         goto EXIT;
      }
      status = AssignKey(clientSession, request->messageBuffer + key);
   }
   *response = tempResponse;


EXIT:
   if (!SUCCESS(status))
   {
      DestroyMessage(&tempResponse);
   }
   free(respBuffer);
   respBuffer = NULL;

   respBuffer = GetErrorMessage(status);
   Log(globals.logger, 5, "[INFO] Login response for user ", request->messageBuffer, " finished with exitcode:", respBuffer, "\n");
   free(respBuffer);
   respBuffer = NULL;


   return status;
}

STATUS HandleUnkownRequest(PMESSAGE* response)
{
   STATUS status;
   PMESSAGE tempResponse;
   LPSTR respBuffer;

   respBuffer = NULL;
   tempResponse = NULL;
   status = EXIT_SUCCESS_STATUS;

   Log(globals.logger, 1, "[INFO] Received unknown request!\n");

   if (NULL == response)
   {
      Log(globals.logger, 1, "[WARNING] Null parameter received. Aborting unknown request execution!\n");
      status = NULL_POINTER;
      goto EXIT;
   }

   status = CreateMessage(&tempResponse);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   tempResponse->messageType = FAILURE_RESPONSE;
   respBuffer = GetErrorMessage(UNKNOWN_REQUEST);
   status = AddBuffer(tempResponse, respBuffer, TRUE);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }
   *response = tempResponse;

EXIT:
   if (!SUCCESS(status))
   {
      DestroyMessage(&tempResponse);
   }
   free(respBuffer);
   respBuffer = NULL;
   return status;
}

STATUS HandleEncryptRequest(PSERVER server, PCLIENT_SESSION clientSession, PMESSAGE request, PMESSAGE* response)
{
   STATUS status;
   PMESSAGE tempResponse; 
   LPSTR respBuffer;

   respBuffer = NULL;
   tempResponse = NULL;
   status = EXIT_SUCCESS_STATUS;
   UNREFERENCED_PARAMETER(server);

   Log(globals.logger, 3, "[INFO] Received encrypt request for user ", clientSession->userName, ".\n");

   if (NULL == response)
   {
      Log(globals.logger, 1, "[WARNING] Null parameter received. Aborting encrypt request execution!\n");
      status = NULL_POINTER;
      goto EXIT;
   }

   status = CreateMessage(&tempResponse);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   if(clientSession->userName == NULL || clientSession->finished)
   {
      tempResponse->messageType = FAILURE_RESPONSE;
      respBuffer = GetErrorMessage(USER_NOT_LOGGED_IN);
      status = AddBuffer(tempResponse, respBuffer, TRUE);
      if (!SUCCESS(status))
      {
         goto EXIT;
      }
   }
   else
   {
      tempResponse->messageType = ENCRYPT_RESPONSE;
      Encrypt(request->messageBuffer, request->messageLength, clientSession->key, clientSession->keySize);
      status = AddBuffer(tempResponse, request->messageBuffer, FALSE);
      if(!SUCCESS(status))
      {
         goto EXIT;
      }
   }

   *response = tempResponse;


EXIT:
   if (!SUCCESS(status))
   {
      DestroyMessage(&tempResponse);
   }
   free(respBuffer);
   respBuffer = NULL;

   respBuffer = GetErrorMessage(status);
   Log(globals.logger, 5, "[INFO] Encrypt response for user ", clientSession->userName, " finished with exitcode:", respBuffer, "\n");
   free(respBuffer);
   respBuffer = NULL;


   return status;
}

STATUS HandleRequest(PSERVER server, PCLIENT_SESSION clientSession, PMESSAGE request, PMESSAGE* response)
{
   STATUS status;

   status = EXIT_SUCCESS_STATUS;

   if (NULL == response)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   switch (request->messageType)
   {
   case LOGIN_REQUEST:
      status = HandleLoginRequest(server, clientSession, request, response);
      break;
   case LOGOUT_REQUEST:
      status = HandleLogoutRequest(server, request, response);
      break;
   case ENCRYPT_REQUEST:
      status = HandleEncryptRequest(server, clientSession, request, response);
      break;
   default:
      status = HandleUnkownRequest(response);
      break;
   }


EXIT:
   return status;
}

DWORD WINAPI ServeClient(LPVOID lpvParam)
{
   PTHREAD_CONTEXT threadContext;
   PSERVER server;
   PCLIENT_SESSION clientSession;
   STATUS status;
   HANDLE pipeHandle;
   LPSTR statusBuffer;
   PMESSAGE request;
   PMESSAGE response;

   server = NULL;
   clientSession = NULL;
   request = NULL;
   response = NULL;
   statusBuffer = NULL;
   pipeHandle = INVALID_HANDLE_VALUE;
   status = EXIT_SUCCESS_STATUS;
   threadContext = NULL;

   if (NULL == lpvParam)
   {
      printf("[INFO] Null parameter received. Aborting execution!\n");
      Log(globals.logger, 1, "[WARNING] Null parameter received. Aborting thread execution!\n");
      status = NULL_POINTER;
      goto EXIT;
   }

   threadContext = (PTHREAD_CONTEXT)lpvParam;
   server = threadContext->server;
   clientSession = threadContext->clientSession;
   pipeHandle = clientSession->pipeHandle;


   while (!clientSession->finished)
   {
      status = ReadMessage(pipeHandle, &request);
      if (!SUCCESS(status))
      {
         goto EXIT;
      }
      clientSession->lastActivity = GetTickCount64();
      status = HandleRequest(server, clientSession, request, &response);
      if (!clientSession->timeout)
      {
         status = WriteMessage(pipeHandle, response);
         clientSession->lastActivity = GetTickCount64();
      }
      DestroyMessage(&request);
      DestroyMessage(&response);
   }


EXIT:
   DisconnectNamedPipe(pipeHandle);

   DestroyMessage(&request);
   DestroyMessage(&response);
   statusBuffer = GetErrorMessage(status);
   Log(globals.logger, 3, "[INFO] Finishing thread execution. Exit code: ", statusBuffer, "\n");
   free(statusBuffer);
   clientSession->finished = TRUE;

   DestroyThreadContext(&threadContext);
   return (DWORD)status;
}

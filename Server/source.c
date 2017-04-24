#include <Windows.h>
#include <stdio.h>


#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  


#include "Server/include/server.h"
#include "Server/include/global_data.h"
#include "User/include/user.h"
#include "Helper/include/constants.h"
#include "Helper/include/strutils.h"
#include "Console/include/console.h"


int main(int argc, char* argv[])
{
   
   STATUS status;
   PSERVER server;
   HANDLE runningThread;
   HANDLE consoleThread;
   LPSTR pipeName;
   LPSTR logFileName;
   DWORD noThreads;
   DWORD noWorkers;
 
   consoleThread = NULL;
   pipeName = NULL;
   logFileName = NULL;
   server = NULL;
   runningThread = NULL;
   status = EXIT_SUCCESS_STATUS;
   noThreads = 8;
   noWorkers = 1;

   if (argc < 3)
   {
      printf("Invalid number of arguments!\n");
      status = -1;
      goto EXIT;
   }
   
   noThreads = GetNumber(argv[1]);
   noWorkers = GetNumber(argv[2]);

   if(argc >= 4)
   {
      pipeName = argv[3];
   }

   if(argc >= 5)
   {
      logFileName = argv[4];
   }

   status = InitGlobalData(logFileName);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   status = CreateServer(&server, pipeName, DEFAULT_USER_FILE_NAME, noThreads, noWorkers);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   runningThread = CreateThread(NULL, 0, Run, server, 0, NULL);
   if(NULL == runningThread)
   {
      status = CREATE_THREAD_FAILED;
      goto EXIT;
   }
   server->runningThread = runningThread;

   consoleThread = CreateThread(NULL, 0, RunConsole, server, 0, NULL);
   if (NULL == runningThread)
   {
      status = CREATE_THREAD_FAILED;
      goto EXIT;
   }

   WaitForSingleObject(consoleThread, INFINITE);
   WaitForSingleObject(runningThread, INFINITE);

EXIT:

   DestroyServer(&server);
   runningThread = NULL;
   consoleThread = NULL;

   printf("Execution terminated with exitcode %d\n", status);
   UnitialiseGlobalData();
   _CrtDumpMemoryLeaks();
   return status;
}

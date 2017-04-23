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

typedef struct USR
{
   char* data;
}USR, *PUSR;

void DestroyUsr(PUSR* a)
{
   if (NULL == a) goto EXIT;
   free(*a);
   *a = NULL;
EXIT:;
}

STATUS execTest(LPVOID param)
{
   PUSR a = (PUSR)(param);
   printf("%s\n", a->data);

   DestroyUsr(&a);
   return EXIT_SUCCESS_STATUS;
}


int main(int argc, char* argv[])
{
   
   STATUS status;
   PSERVER server;
   HANDLE runningThread;
   HANDLE consoleThread;
   LPSTR pipeName;
   LPSTR logFileName;
   DWORD noThreads;
 
   consoleThread = NULL;
   pipeName = NULL;
   logFileName = NULL;
   server = NULL;
   runningThread = NULL;
   status = EXIT_SUCCESS_STATUS;
   noThreads = 8;

   if (argc < 2)
   {
      printf("Invalid number of arguments!\n");
      status = -1;
      goto EXIT;
   }
   
   noThreads = GetNumber(argv[1]);

   if(argc >= 3)
   {
      pipeName = argv[2];
   }

   if(argc >= 4)
   {
      logFileName = argv[3];
   }

   status = InitGlobalData(logFileName);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   status = CreateServer(&server, pipeName, DEFAULT_USER_FILE_NAME, noThreads);
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

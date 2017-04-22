#include <Windows.h>
#include <stdio.h>


#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  


#include "Server/include/server.h"
#include "Server/include/global_data.h"
#include "User/include/user.h"
#include "Helper/include/constants.h"
#include "Utils/include/thread_pool.h"

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
   
   server = NULL;
   runningThread = NULL;
   status = EXIT_SUCCESS_STATUS;

   status = InitGlobalData(NULL);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   status = CreateServer(&server, NULL, DEFAULT_USER_FILE_NAME, 4);
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
   WaitForSingleObject(runningThread, INFINITE);
   
   
   /*
   PTHREAD_POOL thr;
   PUSR a1 = (PUSR)malloc(sizeof(PUSR)); a1->data = "Ana are mere";
   PUSR a2 = (PUSR)malloc(sizeof(PUSR)); a2->data = "Marian nu are mere";
   PUSR a3 = (PUSR)malloc(sizeof(PUSR)); a3->data = "Marian! Il bag in ma-sa de handicapat!";
   PUSR a4 = (PUSR)malloc(sizeof(PUSR)); a4->data = "Mircea Badea";
   status = CreateThreadPool(&thr, 5, execTest, DestroyUsr);
   AddTask(thr, a1);
   AddTask(thr, a2);
   AddTask(thr, a4);
   AddTask(thr, a3);
   DestroyThreadPool(&thr);
   */

EXIT:
   UnitialiseGlobalData();
   DestroyServer(&server);
   runningThread = NULL;

   printf("Execution terminated with exitcode %d\n", status);

   _CrtDumpMemoryLeaks();
   return status;
}

#include "../include/console.h"
#include <stdio.h>

void DisplayMenu()
{
   printf("---------------------\n");
   printf("Available Commands: \n");
   printf("   1 - Display information about current sessions\n");
   printf("   2 - Stop server execution\n");
}

void DisplayInformation(PSERVER server)
{
   DWORD i;
   CRITICAL_SECTION section;

   InitializeCriticalSection(&section);

   for(i = 0; i < server->noMaxIOThreads; ++i)
   {
      EnterCriticalSection(&section);
      
      if(server->loggedClients[i] != NULL && server->loggedClients[i]->userName != NULL)
      {
         printf("Information about user %s\n", server->loggedClients[i]->userName);
         printf("Total encrypted bytes: %llu\n", server->loggedClients[i]->totalBytesEncrypted);
         if (server->loggedClients[i]->finished) 
         {
            printf("Client status: finished\n");
         }
         else if (server->loggedClients[i]->timeout)
         {
            printf("Client status: pending timeout\n");
         }
         else
         {
            printf("Client status: online\n");
         }

      }

      LeaveCriticalSection(&section);
   }

   DeleteCriticalSection(&section);
}


void StopExecution(PSERVER server)
{
   SetStopFlag(server);
}

DWORD WINAPI RunConsole(LPVOID argument)
{
   int opt;
   PSERVER server;

   server = (PSERVER)argument;
   opt = -1;

   do
   {
      DisplayMenu();
      printf("What is your command?\n");
      scanf_s("%d", &opt);
      switch (opt)
      {
      case 1:
         {
         DisplayInformation(server);
         break;
         }
      case 2:
         {
         StopExecution(server);
         break;
         }
      default:
         {
         printf("Invalid command! Try again...\n");
         break;
         }
      }

   } while (opt != 2);

   return 0;
}
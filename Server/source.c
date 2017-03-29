#include <Windows.h>
#include <stdio.h>


#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  


#include "Server/include/server.h"


int main(int argc, char* argv[])
{
   
   STATUS status;
   PSERVER server;
   
   server = NULL;
   status = EXIT_SUCCESS_STATUS;

   status = CreateServer(&server, NULL, NULL);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

EXIT:
   DestroyServer(&server);
   
   printf("Execution terminated with exitcode %d\n", status);
   
   _CrtDumpMemoryLeaks();
   return status;
}

#include <Windows.h>
#include <stdio.h>


#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  


#include "Server/include/server.h"
#include "Server/include/global_data.h"


int main(int argc, char* argv[])
{
   
   STATUS status;
   PSERVER server;
   
   server = NULL;
   status = EXIT_SUCCESS_STATUS;

   status = InitGlobalData(NULL);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   status = CreateServer(&server, NULL);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

EXIT:
   UnitialiseGlobalData();
   DestroyServer(&server);
   
   printf("Execution terminated with exitcode %d\n", status);

   _CrtDumpMemoryLeaks();
   return status;
}

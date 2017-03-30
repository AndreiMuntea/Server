#include "../include/client_session.h"


STATUS CreateClientSession(PCLIENT_SESSION* clientSession, HANDLE pipeHandle)
{
   STATUS status;
   PCLIENT_SESSION tempSession;

   status = EXIT_SUCCESS_STATUS;
   tempSession = NULL;

   if(NULL == clientSession)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   tempSession = (PCLIENT_SESSION)malloc(sizeof(CLIENT_SESSION));
   if(NULL == tempSession)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempSession->pipeHandle = pipeHandle;
   *clientSession = tempSession;

EXIT:
   if(!SUCCESS(status))
   {
      free(tempSession);
      tempSession = NULL;
   }
   return status;
}


void TerminateClientSession(PCLIENT_SESSION* clientSession)
{
   if(NULL == clientSession)
   {
      goto EXIT;
   }

   if(NULL == *clientSession)
   {
      goto EXIT;
   }

   CloseHandle((*clientSession)->pipeHandle);
   free(*clientSession);
   *clientSession = NULL;


EXIT:
   return;
}
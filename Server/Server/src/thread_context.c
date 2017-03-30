#include "../include/thread_context.h"

STATUS CreateThreadContext(PTHREAD_CONTEXT* threadContext, PSERVER server, PCLIENT_SESSION clientSession)
{
   STATUS status;
   PTHREAD_CONTEXT tempThreadContext;

   status = EXIT_SUCCESS_STATUS;
   tempThreadContext = NULL;

   if(NULL == threadContext)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   tempThreadContext = (PTHREAD_CONTEXT)malloc(sizeof(THREAD_CONTEXT));
   if(NULL == tempThreadContext)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempThreadContext->clientSession = clientSession;
   tempThreadContext->server = server;

   *threadContext = tempThreadContext;

EXIT:
   if(!SUCCESS(status))
   {
      free(tempThreadContext);
      tempThreadContext = NULL;
   }
   return status;
}


void DestroyThreadContext(PTHREAD_CONTEXT* threadContext)
{
   DestroyServer(&(*threadContext)->server);
   TerminateClientSession(&(*threadContext)->clientSession);
   free(*threadContext);
   *threadContext = NULL;
}
#include "../include/thread_pool_argument.h"


STATUS CreateThreadPoolArgument(PTHREAD_POOL_ARGUMENT* threadPoolArgument, PENCRYPT_PACKAGE package, PCLIENT_SESSION client)
{
   STATUS status;
   PTHREAD_POOL_ARGUMENT tempThreadPoolArgument;

   status = EXIT_SUCCESS_STATUS;
   tempThreadPoolArgument = NULL;

   if(NULL == threadPoolArgument)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   tempThreadPoolArgument = (PTHREAD_POOL_ARGUMENT)malloc(sizeof(THREAD_POOL_ARGUMENT));
   if(NULL == tempThreadPoolArgument)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempThreadPoolArgument->client = client;
   tempThreadPoolArgument->package = package;

   *threadPoolArgument = tempThreadPoolArgument;

EXIT:
   return status;
}

void DestroyThreadPoolArgument(PTHREAD_POOL_ARGUMENT* threadPoolArgument)
{
   if(NULL == threadPoolArgument)
   {
      goto EXIT;
   }

   if(NULL == *threadPoolArgument)
   {
      goto EXIT;
   }

   free(*threadPoolArgument);
   *threadPoolArgument = NULL;
   
EXIT:
   return;
}
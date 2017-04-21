#include "../include/thread_pool.h"




DWORD WINAPI LaunchThread(LPVOID argument)
{
   PTHREAD_POOL context;
   LPVOID task;
   STATUS status;

   task = NULL;
   status = EXIT_SUCCESS_STATUS;
   context = (PTHREAD_POOL)argument;

   while (!context->pendingShutdown || context->tasksQueue->size > 0)
   {
      status = PopElement(context->tasksQueue, &task);
      if (!SUCCESS(status))
      {
         if (status == EMPTY_QUEUE)
         {
            status = EXIT_SUCCESS_STATUS;
            Sleep(DEFAULT_THREAD_SLEEP);
         }
         else
         {
            //internal error -> stopping
            break;
         }
      }
      else
      {
         status = context->executor(task);
      }
   }

   return status;
}


STATUS CreateThreadPool(
   PTHREAD_POOL* threadPool,
   unsigned int noWorkers,
   STATUS (*executor)(LPVOID),
   void (*taskDestructor)(LPVOID))
{
   STATUS status;
   PTHREAD_POOL tempThreadPool;
   HANDLE* tempHThreadsHandles;
   DWORD* tempDWThreadIdArray;
   PCONCURENT_QUEUE concurentQueue;
   unsigned int i;

   i = 0;
   concurentQueue = NULL;
   tempHThreadsHandles = NULL;
   status = EXIT_SUCCESS_STATUS;
   tempThreadPool = NULL;
   tempDWThreadIdArray = NULL;

   status = CreateConcurentQueue(&concurentQueue, taskDestructor);
   if (!SUCCESS(status))
   {
      goto EXIT;
   }

   tempHThreadsHandles = (HANDLE*)malloc(noWorkers * sizeof(HANDLE));
   if (NULL == tempHThreadsHandles)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempDWThreadIdArray = (DWORD*)malloc(noWorkers * sizeof(DWORD));
   if (NULL == tempDWThreadIdArray)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }


   tempThreadPool = (PTHREAD_POOL)malloc(sizeof(THREAD_POOL));
   if (NULL == tempThreadPool)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempThreadPool->executor = executor;
   tempThreadPool->hThreadsArray = tempHThreadsHandles;
   tempThreadPool->noWorkers = noWorkers;
   tempThreadPool->pendingShutdown = FALSE;
   tempThreadPool->tasksQueue = concurentQueue;
   tempThreadPool->dwThreadIdArray = tempDWThreadIdArray;

   for (i = 0; i < noWorkers; ++i)
   {
      tempHThreadsHandles[i] = CreateThread(NULL, 0, LaunchThread, tempThreadPool, 0, &tempDWThreadIdArray[i]);
      if (NULL == tempHThreadsHandles[i])
      {
         status = CREATE_THREAD_FAILED;
         goto EXIT;
      }
   }

   *threadPool = tempThreadPool;

EXIT:
   if (!SUCCESS(status))
   {
      if (tempThreadPool != NULL)
      {
         DestroyThreadPool(&tempThreadPool);
      }
      else
      {
         free(tempHThreadsHandles);
         free(tempDWThreadIdArray);
         DestroyConcurentQueue(&concurentQueue);
      }
   }
   return status;
}

void DestroyThreadPool(PTHREAD_POOL* threadPool)
{
   unsigned int waitFor, i;
   PTHREAD_POOL tempThreadPool;

   waitFor = 0;
   i = 0;
   tempThreadPool = NULL;

   if (NULL == threadPool)
   {
      goto EXIT;
   }
   if (NULL == *threadPool)
   {
      goto EXIT;
   }

   tempThreadPool = *threadPool;
   for (i = 0; i < tempThreadPool->noWorkers; ++i)
   {
      if (NULL != tempThreadPool->hThreadsArray[i])
      {
         waitFor++;
      }
   }

   AbortExecution(tempThreadPool);
   WaitForMultipleObjects(waitFor, tempThreadPool->hThreadsArray, TRUE, INFINITE);

   for (i = 0; i < tempThreadPool->noWorkers; ++i)
   {
      if (NULL != tempThreadPool->hThreadsArray[i])
      {
         CloseHandle(tempThreadPool->hThreadsArray[i]);
         tempThreadPool->hThreadsArray[i] = NULL;
      }
   }

   free(tempThreadPool->hThreadsArray);
   free(tempThreadPool->dwThreadIdArray);
   DestroyConcurentQueue(&tempThreadPool->tasksQueue);
   free(*threadPool);
   *threadPool = NULL;

EXIT:
   return;
}

void AbortExecution(PTHREAD_POOL thread_pool)
{
   thread_pool->pendingShutdown = TRUE;
}

STATUS AddTask(PTHREAD_POOL threadPool, LPVOID task)
{
   STATUS status;

   status = EXIT_SUCCESS_STATUS;

   if (threadPool->pendingShutdown)
   {
      status = PENDING_SHUTDOWN;
      goto EXIT;
   }

   status = PushElement(threadPool->tasksQueue, task);

EXIT:
   return status;
}

#ifndef UTILS_THREAD_POOL_H
#define UTILS_THREAD_POOL_H

#include <Windows.h>

#include "../../Protocols/include/definitions.h"
#include "concurent_queue.h"

#define DEFAULT_THREAD_SLEEP 1000


typedef struct _THREAD_POOL
{
   unsigned int noWorkers;
   HANDLE* hThreadsArray;
   DWORD* dwThreadIdArray;
   BOOL pendingShutdown;
   STATUS(*executor)(LPVOID);
   PCONCURENT_QUEUE tasksQueue;

}THREAD_POOL, *PTHREAD_POOL;

STATUS CreateThreadPool(
   PTHREAD_POOL* threadPool, 
   unsigned int noWorkers, 
   STATUS(*executor)(LPVOID), 
   void(*taskDestructor)(LPVOID));

void DestroyThreadPool(PTHREAD_POOL* threadPool);

void AbortExecution(PTHREAD_POOL thread_pool);

STATUS AddTask(PTHREAD_POOL threadPool, LPVOID task);

#endif //UTILS_THREAD_POOL_H
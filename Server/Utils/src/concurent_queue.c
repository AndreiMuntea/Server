#include "../include/concurent_queue.h"

STATUS CreateConcurentQueue(PCONCURENT_QUEUE* queue, void(*destructorFunction)(LPVOID))
{
   STATUS status;
   PCONCURENT_QUEUE tempQueue;
   PLIST tempList;
   LPCRITICAL_SECTION criticalSection;

   status = EXIT_SUCCESS_STATUS;
   tempQueue = NULL;
   tempList = NULL;
   criticalSection = NULL;

   criticalSection = (LPCRITICAL_SECTION)malloc(sizeof(CRITICAL_SECTION));
   if (NULL == criticalSection)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   status = CreateList(&tempList, destructorFunction);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

  

   tempQueue = (PCONCURENT_QUEUE)malloc(sizeof(CONCURENT_QUEUE));
   if(NULL == tempQueue)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   InitializeCriticalSection(criticalSection);

   tempQueue->size = 0;
   tempQueue->list = tempList;
   tempQueue->criticalSection = criticalSection;
   *queue = tempQueue;

EXIT:
   if(!SUCCESS(status))
   {
      free(criticalSection);
      DestroyList(&tempList);
      DestroyConcurentQueue(&tempQueue);
   }
   return status;
}

void DestroyConcurentQueue(PCONCURENT_QUEUE* queue)
{
   if(NULL == queue)
   {
      goto EXIT;
   }
   
   if(NULL == *queue)
   {
      goto EXIT;
   }

   DestroyList(&(*queue)->list);
   DeleteCriticalSection((*queue)->criticalSection);
   free((*queue)->criticalSection);
   free((*queue));
   *queue = NULL;

EXIT:
   return;
}

STATUS PopElement(PCONCURENT_QUEUE queue, LPVOID* element)
{
   STATUS status;
   status = EXIT_SUCCESS_STATUS;
   EnterCriticalSection(queue->criticalSection);

   if(queue->size == 0)
   {
      status = EMPTY_QUEUE;
      goto EXIT;
   }

   status = TakeElement(queue->list, 0, element);
   if(SUCCESS(status))
   {
      queue->size--;
   }

EXIT:
   LeaveCriticalSection(queue->criticalSection);
   return status;
}

STATUS PushElement(PCONCURENT_QUEUE queue, LPVOID data)
{
   STATUS status;
   status = EXIT_SUCCESS_STATUS;

   EnterCriticalSection(queue->criticalSection);

   status = AddElement(queue->list, data);
   if(SUCCESS(status))
   {
      queue->size++;
   }

   LeaveCriticalSection(queue->criticalSection);

   return status;
}
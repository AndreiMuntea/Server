#ifndef UTILS_CONCURENT_QUEUE_H
#define UTILS_CONCURENT_QUEUE_H

#include "list.h"

typedef struct _CONCURENT_QUEUE
{
   int size;
   PLIST list;
   LPCRITICAL_SECTION criticalSection;
}CONCURENT_QUEUE, *PCONCURENT_QUEUE;

STATUS CreateConcurentQueue(PCONCURENT_QUEUE* queue, void(*destructorFunction)(LPVOID));

void DestroyConcurentQueue(PCONCURENT_QUEUE* queue);

STATUS PopElement(PCONCURENT_QUEUE queue, LPVOID* element);

STATUS PushElement(PCONCURENT_QUEUE queue, LPVOID data);

#endif //UTILS_CONCURENT_QUEUE_H
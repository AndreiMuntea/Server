#ifndef UTILS_LIST_H
#define UTILS_LIST_H

#include <Windows.h>
#include "../../Protocols/include/definitions.h"

typedef struct _LIST_NODE
{
   LPVOID data;
   struct _LIST_NODE* next;
}LIST_NODE, *PLIST_NODE;

typedef struct _LIST
{
   PLIST_NODE head;
   PLIST_NODE tail;
   int size;
   void (*destructor)(LPVOID);
}LIST, *PLIST;

STATUS GetElement(PLIST list, int index, LPVOID* element);

STATUS TakeElement(PLIST list, int index, LPVOID* element);

STATUS CreateList(PLIST* list, void(*destructorFunction)(LPVOID));

void DestroyList(PLIST* list);

BOOL ExistsElement(PLIST list, LPVOID element, BOOL(*compareFunction)(LPVOID, LPVOID));

STATUS AddElement(PLIST list, LPVOID data);

#endif //UTILS_LIST_H
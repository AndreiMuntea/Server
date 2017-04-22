#include <Windows.h>

#include "../include/list.h"

STATUS CreateNode(LPVOID data, PLIST_NODE* node)
{
   STATUS status;
   PLIST_NODE tempNode;

   tempNode = NULL;
   status = EXIT_SUCCESS_STATUS;

   if (node == NULL)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   tempNode = (PLIST_NODE)malloc(sizeof(LIST_NODE));
   if(NULL == tempNode)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }
   tempNode->next = NULL;
   tempNode->data = data;
   *node = tempNode;


EXIT:
   return status;
}

void DestroyNode(PLIST_NODE* node, void(*destructorFunction)(LPVOID))
{
   if(node == NULL)
   {
      goto EXIT;
   }
   if(*node == NULL)
   {
      goto EXIT;
   }

   destructorFunction(&(*node)->data);
   free(*node);
   node = NULL;

EXIT:
   return;
}


STATUS CreateList(PLIST* list, void (*destructorFunction)(LPVOID))
{
   PLIST tempList;
   STATUS status;

   status = EXIT_SUCCESS_STATUS;
   tempList = NULL;
   
   tempList = (PLIST)malloc(sizeof(LIST));
   if(NULL == tempList)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   tempList->head = NULL;
   tempList->tail = NULL;
   tempList->size = 0;
   tempList->destructor = destructorFunction;
   *list = tempList;

EXIT:
   if(!SUCCESS(status))
   {
      DestroyList(&tempList);
   }
   return status;
}


void DestroyList(PLIST* list)
{
   PLIST_NODE current;
   PLIST_NODE next;
   
   current = NULL;
   next = NULL;

   if(NULL == list)
   {
      goto EXIT;
   }

   if (NULL == *list)
   {
      goto EXIT;
   }

   current = (*list)->head;
   while(current!=NULL)
   {
      next = current->next;
      DestroyNode(&current, (*list)->destructor);
      current = next;
   }
   free(*list);
   *list = NULL;

EXIT:
   return;
}

STATUS AddElement(PLIST list, LPVOID data)
{
   STATUS status;
   PLIST_NODE element;

   element = NULL;
   status = EXIT_SUCCESS_STATUS;

   status = CreateNode(data, &element);
   if(!SUCCESS(status))
   {
      goto EXIT;
   }

   if (NULL == list->head)
   {
      list->head = element;
      list->tail = element;
      list->size = 1;
      goto EXIT;
   }

   list->tail->next = element;
   list->tail = element;
   list->size++;
   

EXIT:
   if(!SUCCESS(status))
   {
      DestroyNode(&element, list->destructor);
   }
   return status;
}



STATUS GetElement(PLIST list, int index, LPVOID* element)
{
   STATUS status;
   PLIST_NODE current;
   int idx;

   idx = 0;
   status = EXIT_SUCCESS_STATUS;
   current = NULL;
   
   if(NULL == element)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   if(index < 0 || index >= list->size)
   {
      status = IDEX_OUT_OF_BOUNDS;
      goto EXIT;
   }

   current = list->head;
   for(idx = 0; idx < index; ++idx)
   {
      current = current->next;
   }

   *element = current->data;
   

EXIT:
   return status;
}

STATUS TakeElement(PLIST list, int index, LPVOID* element)
{
   STATUS status;
   PLIST_NODE current;
   PLIST_NODE prev;
   int idx;

   idx = 0;
   prev = NULL;
   status = EXIT_SUCCESS_STATUS;
   current = NULL;

   if (NULL == element)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   if (index < 0 || index >= list->size)
   {
      status = IDEX_OUT_OF_BOUNDS;
      goto EXIT;
   }

   current = list->head;
   if(index == 0)
   {
      *element = current->data;
      list->head = list->head->next;
      free(current);
      goto EXIT;
   }

   
   for (idx = 0; idx < index; ++idx)
   {
      prev = current;
      current = current->next;
   }
   prev->next = current->next;
   *element = current->data;
   list->size--;
   free(current);
   current = NULL;

EXIT:
   return status;
}

BOOL ExistsElement(PLIST list, LPVOID element, BOOL(*compareFunction)(LPVOID, LPVOID))
{

   PLIST_NODE current;
   BOOL found;
   
   found = FALSE;
   current = NULL;

   if(NULL == list)
   {
      goto EXIT;
   }

   current = list->head;

   while(current != NULL && !found)
   {
      found = compareFunction(element, current->data);
      current = current->next;
   }

 EXIT:
   return found;

}
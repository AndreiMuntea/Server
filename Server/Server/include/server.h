#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H
#include "../../Logger/include/logger.h"


typedef struct _SERVER
{
   LPTSTR pipeName;
   BOOL closeFlag;
   ULONG64 refCounter;

}SERVER, *PSERVER;


STATUS CreateServer(PSERVER* server, LPCTSTR pipeFileName);

PSERVER Reference(PSERVER server);

STATUS Run(PSERVER server);

void DestroyServer(PSERVER* server);

void SetStopFlag(PSERVER server);

DWORD WINAPI ServeClient(LPVOID threadContext);

#endif //SERVER_SERVER_H
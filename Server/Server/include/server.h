#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H
#include "../../Logger/include/logger.h"


typedef struct _SERVER
{
   PLOGGER logger;
   LPTSTR pipeName;
   BOOL closeFlag;

}SERVER, *PSERVER;


STATUS CreateServer(PSERVER* server, LPCTSTR pipeFileName, LPCTSTR logFileName);

void DestroyServer(PSERVER* server);

#endif //SERVER_SERVER_H
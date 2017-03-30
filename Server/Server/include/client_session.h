#ifndef SERVER_CLIENT_SESSION_H
#define SERVER_CLIENT_SESSION_H

#include <Windows.h>
#include "../../Utils/include/definitions.h"

typedef struct _CLIENT_SESSION
{
   HANDLE pipeHandle;
}CLIENT_SESSION, *PCLIENT_SESSION;


STATUS CreateClientSession(PCLIENT_SESSION* clientSession, HANDLE pipeHandle);

void TerminateClientSession(PCLIENT_SESSION* clientSession);


#endif //SERVER_CLIENT_SESSION_H
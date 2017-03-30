#ifndef SERVER_THREAD_CONTEXT_H
#define SERVER_THREAD_CONTEXT_H

#include "client_session.h"
#include "server.h"

typedef struct _THREAD_CONTEXT
{
   PSERVER server;
   PCLIENT_SESSION clientSession;

}THREAD_CONTEXT, *PTHREAD_CONTEXT;


STATUS CreateThreadContext(PTHREAD_CONTEXT* threadContext, PSERVER server, PCLIENT_SESSION clientSession);

void DestroyThreadContext(PTHREAD_CONTEXT* threadContext);

#endif //SERVER_THREAD_CONTEXT_H
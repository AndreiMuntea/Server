#ifndef SERVER_THREAD_CONTEXT_H
#define SERVER_THREAD_CONTEXT_H

#include "client_session.h"
#include "server.h"

typedef struct _THREAD_CONTEXT
{
   PSERVER server;
   PCLIENT_SESSION clientSession;

}THREAD_CONTEXT, *PTHREAD_CONTEXT;


/*
* Creates a new instance of a PTHREAD_CONTEXT
* @params: threadContext - a PTHREAD_CONTEXT*
*          server        - a PSERVER
*          clientSession - a PCLIENT_SESSION
* @returns: STATUS - EXIT_SUCCESS_STATUS if instance is created without errors
*                  - error code otherwise
*/
STATUS CreateThreadContext(PTHREAD_CONTEXT* threadContext, PSERVER server, PCLIENT_SESSION clientSession);


/**
* Destroys an instance of a THREAD_CONTEXT
* @params: threadContext - a PTHREAD_CONTEXT*, the instance to be destroyed
*/
void DestroyThreadContext(PTHREAD_CONTEXT* threadContext);

#endif //SERVER_THREAD_CONTEXT_H
#ifndef SERVER_CLIENT_SESSION_H
#define SERVER_CLIENT_SESSION_H

#include <Windows.h>
#include "../../Utils/include/definitions.h"

typedef struct _CLIENT_SESSION
{
   HANDLE pipeHandle;
}CLIENT_SESSION, *PCLIENT_SESSION;


/*
 * Creates a new instance of a CLIENT_SESSION
 * @params: clientSession - a PCLIENT_SESSION*
 *          pipeHandle    - a HANDLE
 * @returns: STATUS - EXIT_SUCCESS_STATUS if instance is created without errors
 *                  - error code otherwise
 */
STATUS CreateClientSession(PCLIENT_SESSION* clientSession, HANDLE pipeHandle);

/**
 * Destroys an instance of a CLIENT_SESSION
 * @params: clientSession - a PCLIENT_SESSION*, the instance to be destroyed
 */
void TerminateClientSession(PCLIENT_SESSION* clientSession);


#endif //SERVER_CLIENT_SESSION_H
#ifndef SERVER_CLIENT_SESSION_H
#define SERVER_CLIENT_SESSION_H

#include <Windows.h>
#include "../../Protocols/include/definitions.h"


typedef struct _CLIENT_SESSION
{
   DWORD sessionID;
   HANDLE pipeHandle;
   ULONG64 lastActivity;
   BOOL finished;
   BOOL timeout;
   LPSTR userName;
   LPSTR key;
   DWORD keySize;
}CLIENT_SESSION, *PCLIENT_SESSION;


/*
 * Creates a new instance of a CLIENT_SESSION
 * @params: clientSession - a PCLIENT_SESSION*
 *          pipeHandle    - a HANDLE
 *          sessionID     - a DWORD, id of the session
 * @returns: STATUS - EXIT_SUCCESS_STATUS if instance is created without errors
 *                  - error code otherwise
 */
STATUS CreateClientSession(PCLIENT_SESSION* clientSession, HANDLE pipeHandle, DWORD sessionID);

/**
 * Destroys an instance of a CLIENT_SESSION
 * @params: clientSession - a PCLIENT_SESSION*, the instance to be destroyed
 */
void TerminateClientSession(PCLIENT_SESSION* clientSession);

STATUS AssignUserName(PCLIENT_SESSION client_session, LPSTR userName);

STATUS AssignKey(PCLIENT_SESSION client_session, LPSTR key);

#endif //SERVER_CLIENT_SESSION_H
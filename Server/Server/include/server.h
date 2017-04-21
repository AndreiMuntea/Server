#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H
#include "../../Logger/include/logger.h"
#include "../../Utils/include/list.h"


typedef struct _SERVER
{
   LPSTR pipeName;
   BOOL closeFlag;
   ULONG64 refCounter;
   PLIST users;

}SERVER, *PSERVER;


/*
* Creates a new instance of a SERVER
* @params: server - a PSERVER*
* @params: pipeFileName - a LPCSTR, the pipe name
*                       - if NULL, the default name will be used instead
*          usersFileName - a LPCSTR the name of the file containing all users
* @returns: STATUS - EXIT_SUCCESS_STATUS if instance is created without errors
*                  - error code otherwise
*/
STATUS CreateServer(PSERVER* server, LPCSTR pipeFileName, LPCSTR usersFileName);


/*
* Increments counter of shared references for an instance of a SERVER
* @params: server - a PSERVER*
* @returns: PSERVER - the instance after incrementing the counter
* @warning: undefined behaviour if given parameter isn't a valid instance of SERVER
*/
PSERVER Reference(PSERVER server);

/*
 * The accepting clients function
 * @params: server  - a PSERVER*
 * @returns: STATUS - the exitcode of function
 */
STATUS Run(PSERVER server);

/**
* Destroys an instance of a SERVER
* @params: server - a PSERVER*, the instance to be destroyed
*/
void DestroyServer(PSERVER* server);

/*
* Sets the stop flag.
* If the stop flag is set, no more clients can connect to server.
* @params: server - a PSERVER*
* @returns: PSERVER - the instance after setting the stop flag
* @warning: undefined behaviour if given parameter isn't a valid instance of SERVER
*/
void SetStopFlag(PSERVER server);

/*
 * A method for serving clients
 * @params: lpvParam - a LPVOID, must be a THREAD_CONTEXT otherwise the function will fail
 * @returns: DWORD - the exitcode of function
 */
DWORD WINAPI ServeClient(LPVOID lpvParam);

#endif //SERVER_SERVER_H
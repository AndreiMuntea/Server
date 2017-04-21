#ifndef SERVER_GLOBAL_DATA_H
#define SERVER_GLOBAL_DATA_H
#include "../../Logger/include/logger.h"


typedef struct _GLOBAL_DATA
{
   PLOGGER logger;
}GLOBAL_DATA, *PGLOBAL_DATA;

extern GLOBAL_DATA globals;

/*
* Initialise the global data
* @params: logFileName - a LPCTSTR, the name of log file
* 
* @returns: STATUS - EXIT_SUCCESS_STATUS if global data is initialised without errors
*                  - error code otherwise
*/
STATUS InitGlobalData(LPCSTR logFileName);

/*
 * Resets the global data
 * Deallocate every resource used by global data
 */
void UnitialiseGlobalData();

#endif //SERVER_GLOBAL_DATA_H
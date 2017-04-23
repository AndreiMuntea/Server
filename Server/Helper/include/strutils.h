#ifndef UTILS_STRUTILS_H
#define UTILS_STRUTILS_H

#include <Windows.h>

#include "../../Protocols/include/definitions.h"

/*
 * Creates the pipe name appending the specified prefix;
 * @params: dest   - a LPSTR*, the destination string
 *          source - a LPCSTR, the source string, if NULL, the default value will be used
 * @returns: STATUS - EXIT_SUCCESS_STATUS if successful
 *                  - error code otherwise 
 */
STATUS CreatePipeName(LPSTR* dest, LPCSTR source);

/*
 * Gets the message for the coresponding status
 */
LPSTR GetErrorMessage(STATUS status);

DWORD GetNumber(LPSTR buffer);

STATUS NumberToString(LPSTR* dest, DWORD number);

#endif //UTILS_STRUTILS_H
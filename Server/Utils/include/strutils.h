#ifndef UTILS_STRUTILS_H
#define UTILS_STRUTILS_H

#include <Windows.h>

#include "definitions.h"

/*
 * Creates the pipe name appending the specified prefix;
 * @params: dest   - a LPSTR*, the destination string
 *          source - a LPCSTR, the source string, if NULL, the default value will be used
 * @returns: STATUS - EXIT_SUCCESS_STATUS if successful
 *                  - error code otherwise 
 */
STATUS CreatePipeName(LPTSTR* dest, LPCTSTR source);

#endif //UTILS_STRUTILS_H
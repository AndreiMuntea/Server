#include <Strsafe.h>

#include "../include/strutils.h"
#include "../include/constants.h"


STATUS CreatePipeName(LPTSTR* dest, LPCTSTR source)
{
   STATUS status;
   LPTSTR temp;
   LPCTSTR copySource;
   size_t finalSize;
   size_t prefixSize;
   size_t sourceSize;
   HRESULT result;

   result = S_OK;
   prefixSize = 0;
   sourceSize = 0;
   finalSize = 0;
   temp = NULL;
   copySource = NULL;
   status = EXIT_SUCCESS_STATUS;

   if (NULL == dest)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   copySource = source;
   if (NULL == source)
   {
      copySource = PIPE_DEFAULT_NAME;
   }

   // Source size
   if (StringCchLength(copySource,STRSAFE_MAX_CCH, &sourceSize) != S_OK)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }

   // Prefix size;
   if (StringCchLength(PIPE_PREFIX,STRSAFE_MAX_CCH, &prefixSize) != S_OK)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }

   // Size of string containing full name of pipe
   finalSize = sourceSize + prefixSize + 1;
   if (finalSize > PIPE_NAME_MAX_SIZE)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }

   temp = (LPTSTR)malloc(finalSize * sizeof(TCHAR));
   if (NULL == temp)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   // Copy the pipe prefix
   result = StringCchCopy(temp, finalSize, PIPE_PREFIX);
   if (result != S_OK)
   {
      status = result;
      goto EXIT;
   }

   // Copy the actual pipe name
   result = StringCchCat(temp, finalSize, copySource);
   if (result != S_OK)
   {
      status = result;
      goto EXIT;
   }

   *dest = temp;

EXIT:
   if (!SUCCESS(status))
   {
      free(temp);
      temp = NULL;
   }

   return status;
}

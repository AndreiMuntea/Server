#include <Strsafe.h>

#include "../include/strutils.h"
#include "../include/constants.h"


STATUS CreatePipeName(LPSTR* dest, LPCSTR source)
{
   STATUS status;
   LPSTR temp;
   LPCSTR copySource;
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
   if (StringCchLengthA(copySource,STRSAFE_MAX_CCH, &sourceSize) != S_OK)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }

   // Prefix size;
   if (StringCchLengthA(PIPE_PREFIX,STRSAFE_MAX_CCH, &prefixSize) != S_OK)
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

   temp = (LPSTR)malloc(finalSize * sizeof(CHAR));
   if (NULL == temp)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   // Copy the pipe prefix
   result = StringCchCopyA(temp, finalSize, PIPE_PREFIX);
   if (result != S_OK)
   {
      status = result;
      goto EXIT;
   }

   // Copy the actual pipe name
   result = StringCchCatA(temp, finalSize, copySource);
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

CHAR GetDigit(int digit)
{
   CHAR res;
   if (digit < 10) res = (CHAR)('0' + digit);
   else res = (CHAR)('A' + digit - 10);
   return res;
}


LPSTR GetErrorMessage(STATUS status)
{
   LPSTR buff;
   size_t i;

   buff = NULL;
   i = 0;

   buff = (LPSTR)malloc(11 * sizeof(CHAR));
   if(buff == NULL)
   {
      goto EXIT;
   }

   buff[0] = '0';
   buff[1] = 'x';
   buff[10] = '\0';
   i = 9;

   while(status > 0)
   {
      buff[i] = GetDigit(status % 16);
      status /= 16;
      --i;
   }

   while (i >= 2)
   {
      buff[i] = GetDigit(0);
      --i;
   } 

EXIT:
   return buff;
}

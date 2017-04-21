#include <Strsafe.h>

#include "../include/protocols.h"


STATUS WriteMessage(HANDLE pipeHandle, PMESSAGE message)
{
   STATUS status;
   BOOL successOperation;
   DWORD cbWritten;
   DWORD cbToWrite;
   size_t bufferSize;

   bufferSize = 0;
   cbToWrite = 0;
   cbWritten = 0;
   successOperation = TRUE;
   status = EXIT_SUCCESS_STATUS;

   if (StringCchLengthA(message->messageBuffer, STRSAFE_MAX_CCH, &bufferSize) != S_OK)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }

   cbToWrite = sizeof(MESSAGE_TYPE) + sizeof(DWORD) + bufferSize * sizeof(CHAR);

   successOperation = WriteFile(
      pipeHandle,
      message,
      cbToWrite,
      &cbWritten,
      NULL
   );

   if (!successOperation || cbToWrite != cbWritten)
   {
      status = GetLastError();
   }

EXIT:
   return status;
}

STATUS BufferedRead(HANDLE pipeHandle, PMESSAGE* message);

STATUS ReadMessage(HANDLE pipeHandle, PMESSAGE* message);



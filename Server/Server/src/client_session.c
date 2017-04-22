#include "../include/client_session.h"
#include <Strsafe.h>


STATUS CreateClientSession(PCLIENT_SESSION* clientSession, HANDLE pipeHandle, DWORD sessionID)
{
   STATUS status;
   PCLIENT_SESSION tempSession;

   status = EXIT_SUCCESS_STATUS;
   tempSession = NULL;

   if (NULL == clientSession)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   tempSession = (PCLIENT_SESSION)malloc(sizeof(CLIENT_SESSION));
   if (NULL == tempSession)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempSession->pipeHandle = pipeHandle;
   tempSession->sessionID = sessionID;
   tempSession->lastActivity = GetTickCount64();
   tempSession->finished = FALSE;
   tempSession->userName = NULL;
   tempSession->key = NULL;
   tempSession->keySize = 0;
   tempSession->timeout = FALSE;

   *clientSession = tempSession;

EXIT:
   if (!SUCCESS(status))
   {
      free(tempSession);
      tempSession = NULL;
   }
   return status;
}


void TerminateClientSession(PCLIENT_SESSION* clientSession)
{
   if (NULL == clientSession)
   {
      goto EXIT;
   }

   if (NULL == *clientSession)
   {
      goto EXIT;
   }

   if ((*clientSession)->pipeHandle != INVALID_HANDLE_VALUE)
   {
      CloseHandle((*clientSession)->pipeHandle);
   }
   free((*clientSession)->userName);
   free((*clientSession)->key);
   free(*clientSession);
   
   *clientSession = NULL;


EXIT:
   return;
}

STATUS AssignUserName(PCLIENT_SESSION client_session, LPSTR userName)
{
   STATUS status;
   LPSTR tempUserName;
   size_t userNameLength;
   HRESULT res;

   res = S_OK;
   userNameLength = 0;
   status = EXIT_SUCCESS_STATUS;
   tempUserName = NULL;

   if (StringCchLengthA(userName, STRSAFE_MAX_CCH, &userNameLength) != S_OK)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }
   userNameLength++;

   tempUserName = (LPSTR)malloc(userNameLength * sizeof(CHAR));
   if (NULL == tempUserName)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   res = StringCchCopyA(tempUserName, userNameLength, userName);
   if (res != S_OK)
   {
      status = res;
      goto EXIT;
   }

   client_session->userName = tempUserName;


EXIT:
   if (!SUCCESS(status))
   {
      free(tempUserName);
   }
   return status;
}

STATUS AssignKey(PCLIENT_SESSION client_session, LPSTR key)
{
   STATUS status;
   LPSTR tempKey;
   size_t keyLength;
   HRESULT res;

   res = S_OK;
   keyLength = 0;
   status = EXIT_SUCCESS_STATUS;
   tempKey = NULL;

   if (StringCchLengthA(key, STRSAFE_MAX_CCH, &keyLength) != S_OK)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }
   keyLength++;

   tempKey = (LPSTR)malloc(keyLength * sizeof(CHAR));
   if (NULL == tempKey)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   res = StringCchCopyA(tempKey, keyLength, key);
   if (res != S_OK)
   {
      status = res;
      goto EXIT;
   }

   client_session->keySize = keyLength;
   client_session->key = tempKey;


EXIT:
   if (!SUCCESS(status))
   {
      free(tempKey);
   }
   return status;
}

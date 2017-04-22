#include <Strsafe.h>

#include "../include/user.h"



STATUS CreateUser(PUSER* user, LPCSTR userName, LPCSTR password)
{
   STATUS status;
   PUSER tempUser;
   LPSTR tempUserName;
   LPSTR tempPassword;
   size_t userNameLength;
   size_t passwordLength;
   HRESULT res;

   res = S_OK;
   userNameLength = 0;
   passwordLength = 0;
   tempUser = NULL;
   tempUserName = NULL;
   tempPassword = NULL;
   status = EXIT_SUCCESS_STATUS;

   if(NULL == user)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   if (StringCchLengthA(userName, STRSAFE_MAX_CCH, &userNameLength) != S_OK)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }
   userNameLength++;

   if (StringCchLengthA(password, STRSAFE_MAX_CCH, &passwordLength) != S_OK)
   {
      status = INVALID_PARAMETER;
      goto EXIT;
   }
   passwordLength++;

   tempUserName = (LPSTR)malloc((userNameLength) * sizeof(CHAR));
   if (NULL == tempUserName)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempPassword = (LPSTR)malloc((passwordLength) * sizeof(CHAR));
   if (NULL == tempPassword)
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

   res = StringCchCopyA(tempPassword, passwordLength, password);
   if (res != S_OK)
   {
      status = res;
      goto EXIT;
   }

   tempUser = (PUSER)malloc(sizeof(USER));
   if(NULL == tempUser)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempUser->userName = tempUserName;
   tempUser->password = tempPassword;

   *user = tempUser;
   

EXIT:
   if(!SUCCESS(status))
   {
      free(tempUserName);
      free(tempPassword);
      DestroyUser(&tempUser);
   }
   return status;
}

void DestroyUser(PUSER* user)
{
   if(NULL == user)
   {
      goto EXIT;
   }

   if(NULL == *user)
   {
      goto EXIT;
   }

   free((*user)->userName);
   free((*user)->password);
   free(*user);
   *user = NULL;

EXIT:
   return;
}


BOOL EqualUsers(PUSER firstUser, PUSER secondUser)
{
   BOOL res;
   
   res = (strcmp(firstUser->userName, secondUser->userName) == 0) &&
      (strcmp(firstUser->password, secondUser->password) == 0);

   return res;
}
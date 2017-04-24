#include "../include/encrypt_package.h"

STATUS CreateEncryptPackage(PENCRYPT_PACKAGE* encryptPackage, PMESSAGE message)
{
   STATUS status;
   PENCRYPT_PACKAGE tempPackage;
   
   status = EXIT_SUCCESS_STATUS;
   tempPackage = NULL;

   if(NULL == encryptPackage)
   {
      status = NULL_POINTER;
      goto EXIT;
   }

   tempPackage = (PENCRYPT_PACKAGE)malloc(sizeof(ENCRYPT_PACKAGE));
   if(NULL == tempPackage)
   {
      status = BAD_ALLOCATION;
      goto EXIT;
   }

   tempPackage->encrypted = FALSE;
   tempPackage->message = message;

   *encryptPackage = tempPackage;

EXIT:
   return status;
}


void DestroyEncryptPackage(PENCRYPT_PACKAGE* encryptPackage)
{
   if(NULL == encryptPackage)
   {
      goto EXIT;
   }

   if(NULL == *encryptPackage)
   {
      goto EXIT;
   }

   DestroyMessage(&(*encryptPackage)->message);
   free(*encryptPackage);
   *encryptPackage = NULL;

EXIT:
   return;
}

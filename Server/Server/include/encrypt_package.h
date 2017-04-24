#ifndef SERVER_ENCRYPT_PACKAGE_H
#define SERVER_ENCRYPT_PACKAGE_H

#include "../../Protocols/include/protocols.h"
#include "client_session.h"

typedef struct _ENCRYPT_PACKAGE
{
   BOOL encrypted;
   PMESSAGE message;
}ENCRYPT_PACKAGE, *PENCRYPT_PACKAGE;

STATUS CreateEncryptPackage(PENCRYPT_PACKAGE* encryptPackage, PMESSAGE message);

void DestroyEncryptPackage(PENCRYPT_PACKAGE* encryptPackage);

#endif //SERVER_ENCRYPT_PACKAGE_H
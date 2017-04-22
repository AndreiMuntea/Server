#ifndef USER_USER_H
#define USER_USER_H

#include <Windows.h>

#include "../../Protocols/include/definitions.h"

typedef struct _USER
{
   LPSTR userName;
   LPSTR password;
}USER, *PUSER;

STATUS CreateUser(PUSER* user, LPCSTR userName, LPCSTR password);

BOOL EqualUsers(PUSER firstUser, PUSER secondUser);

void DestroyUser(PUSER* user);

#endif //USER_USER_H
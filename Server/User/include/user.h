#ifndef USER_USER_H
#define USER_USER_H

#include <Windows.h>

#include "../../Protocols/include/definitions.h"

typedef struct _USER
{
   LPSTR userName;
   LPSTR password;
   BOOL loggedIn;
}USER, *PUSER;

STATUS CreateUser(PUSER* user, LPCSTR userName, LPCSTR password);

void DestroyUser(PUSER* user);

#endif //USER_USER_H
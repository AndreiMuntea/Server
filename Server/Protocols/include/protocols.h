#ifndef PROTOCOLS_PROTOCOLS_H
#define PROTOCOLS_PROTOCOLS_H

#include <Windows.h>

#include "definitions.h"

typedef enum _REQUEST_TYPE
{
   LOGIN_REQUEST, LOGOUT_REQUEST, INITILIASE_CONNECTION_REQUEST, ENCRYPT_REQUEST
}REQUEST_TYPE;

typedef enum _RESPONSE_TYPE
{
   OK_RESPONSE, FAILURE_RESPONSE, ENCRYPT_RESPONSE
}RESPONSE_TYPE;

typedef union _MESSAGE_TYPE
{
   REQUEST_TYPE request;
   RESPONSE_TYPE response;
}MESSAGE_TYPE;

typedef struct _MESSAGE
{
   MESSAGE_TYPE messageType;
   DWORD messageLength;
   LPSTR messageBuffer;

}MESSAGE, *PMESSAGE;


STATUS WriteMessage(HANDLE pipeHandle, PMESSAGE message);

STATUS ReadMessage(HANDLE pipeHandle, PMESSAGE* message);

#endif //PROTOCOLS_PROTOCOLS_H

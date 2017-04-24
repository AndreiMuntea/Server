#ifndef SERVER_THREAD_POOL_ARGUMENT_H
#define SERVER_THREAD_POOL_ARGUMENT_H
#include "encrypt_package.h"


typedef struct _THREAD_POOL_ARGUMENT
{
   PENCRYPT_PACKAGE package;
   PCLIENT_SESSION client;
}THREAD_POOL_ARGUMENT, *PTHREAD_POOL_ARGUMENT;

STATUS CreateThreadPoolArgument(PTHREAD_POOL_ARGUMENT* threadPoolArgument, PENCRYPT_PACKAGE package, PCLIENT_SESSION client);

void DestroyThreadPoolArgument(PTHREAD_POOL_ARGUMENT* threadPoolArgument);

#endif //SERVER_THREAD_POOL_ARGUMENT_H
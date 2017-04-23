#ifndef CONSOLE_CONSOLE_H
#define CONSOLE_CONSOLE_H

#include <Windows.h>
#include "../../Server/include/server.h"

void DisplayInformation(PSERVER server);

void StopExecution(PSERVER server);

DWORD WINAPI RunConsole(LPVOID argument);
#endif // CONSOLE_CONSOLE_H
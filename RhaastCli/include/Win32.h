#ifndef RHAAST_WIN32_H
#define RHAAST_WIN32_H

#include <windows.h>
#include <Types.h>

BOOL ProcessQueryNameById(
    IN  ULONG   Pid,
    OUT PBUFFER Name
);

#endif // RHAAST_WIN32_H
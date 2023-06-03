#ifndef RHAAST_WIN32_H
#define RHAAST_WIN32_H

#include <windows.h>
#include <Types.h>

BOOL ProcessCheckById(
    _In_      ULONG   Pid,
    _Out_opt_ PBUFFER Name
);

#endif // RHAAST_WIN32_H
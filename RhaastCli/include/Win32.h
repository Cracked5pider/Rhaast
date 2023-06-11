#ifndef RHAAST_WIN32_H
#define RHAAST_WIN32_H

#include <windows.h>
#include <Types.h>

#define C_PTR( x )  ( ( PVOID )     ( x ) )
#define U_PTR( x )  ( ( ULONG_PTR ) ( x ) )

BOOL ProcessCheckById(
    _In_      ULONG   Pid,
    _Out_opt_ PBUFFER Name
);

#endif // RHAAST_WIN32_H
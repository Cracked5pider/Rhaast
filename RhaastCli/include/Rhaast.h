#ifndef RHAAST_RHAAST_H
#define RHAAST_RHAAST_H

/* import system libs */
#include <windows.h>
#include <ntstatus.h>
#include <winioctl.h>
#include <ntdef.h>
#include <psapi.h>
#include <tlhelp32.h>

/* import c++ headers */
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

/* import rhaast headers */
#include <Types.h>
#include <Command.h>
#include <Utils.h>
#include <Win32.h>

/* import 3rd party headers */
#include <lib/spdlog/spdlog.h>

class Rhaast
{
private:
    HANDLE      Handle;
    std::string Name;
    BOOL        ExitNow;

public:
    Rhaast(
        std::string DriverName
    );

    ~Rhaast(
        VOID
    );

    BOOL IsConnected(
        VOID
    );

    VOID Routine(
        VOID
    );

private:
    VOID FmtErrorNotConnected(
        VOID
    );

    VOID FmtShowHelp(
        VOID
    );

    VOID FmtNotEnoughArgs(
        std::string command
    );

private:
    VOID CallbackEnumList(
        RS_CALLBACK_TYPE  Type,
        PRS_CALLBACK_DATA Data
    );

private:
    BOOL RhaastConnect(
        std::string DriverName
    );


    BOOL DispatchInput(
        std::vector<std::string> args
    );

    NTSTATUS RhaastSend(
        ULONG Command,
        PVOID Buffer,
        ULONG Size,
        PVOID Response,
        ULONG ResponseSize
    );

    VOID Exit(
        VOID
    );
};

#endif
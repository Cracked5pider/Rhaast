#pragma once

/* typecast hack */
#define C_PTR( x ) ( ( PVOID )    ( x ) )
#define U_PTR( x ) ( ( UINT_PTR ) ( x ) )

/* debug macros */
#define PRINTF( fmt, ... )  DbgPrintEx( 0, 0, "[RHAAST] " fmt, __VA_ARGS__ );
#define PUTS( s )           DbgPrintEx( 0, 0, "[RHAAST] %s\n", s );

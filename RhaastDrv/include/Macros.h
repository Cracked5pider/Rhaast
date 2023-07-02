#pragma once

/* typecast hack */
#define C_PTR( x ) ( ( PVOID )    ( x ) )
#define U_PTR( x ) ( ( UINT_PTR ) ( x ) )

/* dereference pointers */
#define DREF_PTR( x )   ( ( PVOID     ) *( PVOID*   ) ( x ) )
#define DREF_UPTR( x )  ( ( ULONG_PTR ) *( PVOID*   ) ( x ) )

#define DREF_U8( x )    ( ( UCHAR  ) *( PUCHAR*  ) ( x ) )
#define DREF_U16( x )   ( ( USHORT ) *( PUSHORT* ) ( x ) )
#define DREF_U32( x )   ( ( UINT32 ) *( PUINT32* ) ( x ) )

/* debug macros */
#define PRINTF( fmt, ... )  DbgPrintEx( 0, 0, "[RHAAST] " fmt, __VA_ARGS__ );
#define PUTS( s )           DbgPrintEx( 0, 0, "[RHAAST] %s\n", s );

#define MAX_PATH 260
cmake_minimum_required( VERSION 3.25 )
project( RhaastCli )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY E:\\Shared-Folder )
set( CMAKE_CXX_FLAGS "-static -static-libgcc -static-libstdc++" )
# include directory
include_directories( include )

# rhaast cli source
set( RHAAST_SOURCE
    src/Main.cc
    src/Rhaast.cc
    src/Utils.cc
    src/Win32.cc
)

add_executable( RhaastCli ${RHAAST_SOURCE} )

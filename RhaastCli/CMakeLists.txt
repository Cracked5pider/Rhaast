cmake_minimum_required( VERSION 3.25 )
project( RhaastCli )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY .. )

# include directory
include_directories( include )

# rhaast cli source
set( RHAAST_SOURCE
    src/Main.cc
    src/Rhaast.cc
    src/Utils.cc
)

add_executable( RhaastCli ${RHAAST_SOURCE} )

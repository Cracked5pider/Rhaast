cmake_minimum_required( VERSION 3.25 )
project( RhaastClient )

# set some cmake settings
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY .. )
set( CMAKE_PREFIX_PATH "C:\\Qt\\6.5.0\\mingw_64" )

# set qt settings + link to it
set( QT_VERSION 6 )
set( REQUIRED_LIBS Core Gui Widgets )

# source files
set( RHAAST_SRC
    src/Main.cc
)

# include directory
include_directories( include )

# add compile time define
add_definitions( -DQT_NO_DEBUG_OUTPUT )

# add everything to an executable
add_executable( RhaastClient ${RHAAST_SRC} )

# link libraries
target_link_libraries( RhaastClient Qt::Core Qt::Gui Qt::Widgets )

# find packages
find_package( Qt${QT_VERSION} COMPONENTS ${REQUIRED_LIBS} REQUIRED )


cmake_minimum_required( VERSION 3.20 )
project( sow-freecam )

# C++20
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

# Glob recursively
file( GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp" )
include_directories( "src" )

if(MSVC)
    add_compile_options(
        $<$<CONFIG:>:/MT> #---------|
        $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
        $<$<CONFIG:Release>:/MT> #--|
    )
endif()

# Shared library
add_library( sow-freecam SHARED ${SOURCES} )


cmake_minimum_required(VERSION 3.5.1)

project(AUDSRCLIB)

option(BUILD_SHARED_LIB "Build libaudeering as a shared object." OFF)

# set C standard to version 99 for all targets
set(CMAKE_C_STANDARD 99)

# List of source files: libaudsrc_SOURCES
set(libaudsrc_SOURCES
    src/audsrc.c
)

include_directories(
	${INCLUDE_DIRECTORIES},
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(BUILD_SHARED_LIB)
    add_library(audsrc SHARED ${libaudsrc_SOURCES})
else()
    add_library(audsrc STATIC ${libaudsrc_SOURCES})
endif()

# allow linking of libaudsrc into shared libraries
set_property(TARGET audsrc PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(audsrc
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_executable(sr-converter sr-converter.c)

find_library(audeering audeering ${CMAKE_LIBRARY_PATH})
target_link_libraries(sr-converter PRIVATE audeering)

target_link_libraries(sr-converter PRIVATE audsrc)

find_library(soxr soxr ${CMAKE_LIBRARY_PATH})
target_link_libraries(sr-converter PRIVATE soxr)
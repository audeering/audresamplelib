cmake_minimum_required(VERSION 3.5.1)

project(AUDRESAMPLE)

option(BUILD_SHARED_LIB "Build libaudresample as a shared object." OFF)

# set C standard to version 99 for all targets
set(CMAKE_C_STANDARD 99)

# List of source files: libaudresample_SOURCES
set(libaudresample_SOURCES
    src/audresample.c
)

include_directories(
	${INCLUDE_DIRECTORIES},
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if(BUILD_SHARED_LIB)
    add_library(audresample SHARED ${libaudresample_SOURCES})
else()
    add_library(audresample STATIC ${libaudresample_SOURCES})
endif()

# allow linking of libaudresample into shared libraries
set_property(TARGET audresample PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(audresample
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_executable(sr-converter sr-converter.c)

find_library(audeering audeering ${CMAKE_LIBRARY_PATH})
target_link_libraries(sr-converter PRIVATE audeering)

target_link_libraries(sr-converter PRIVATE audresample)

find_library(soxr soxr ${CMAKE_LIBRARY_PATH})
target_link_libraries(sr-converter PRIVATE soxr)
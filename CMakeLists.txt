cmake_minimum_required(VERSION 3.5.1)
if(NOT ${CMAKE_VERSION} VERSION_LESS "3.15")
    cmake_policy(SET CMP0091 NEW) # required for CMAKE_MSVC_RUNTIME_LIBRARY but it's only supported by CMake 3.15 or later
endif()

project(AUDRESAMPLE)

option(BUILD_SHARED_LIB "Build libaudresample as a shared object." OFF)
option(BUILD_TOOLS "Build utilities in the progsrc folder." OFF)


# setup conan
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(KEEP_RPATHS)
endif()


# set C standard to version 99 for all targets
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# List of source files: libaudresample_SOURCES
set(libaudresample_SOURCES
    src/audresample.c
    src/audresample.def
)

if(BUILD_SHARED_LIB)
    add_library(audresample SHARED ${libaudresample_SOURCES})
else()
    add_library(audresample STATIC ${libaudresample_SOURCES})
endif()


# set __WINDOWS directive on Windows platforms
if(WIN32)
    target_compile_definitions(audresample
        PUBLIC
            -D__WINDOWS
            -D_CRT_SECURE_NO_WARNINGS # disable warnings for using unsafe functions in Visual C++ runtime
            -D_CRT_NONSTDC_NO_DEPRECATE # disable warnings for non-standard POSIX functions like strdup
    )
endif()

# allow linking of libaudresample into shared libraries
if(DEFINED CMAKE_POSITION_INDEPENDENT_CODE) # checks if the option was set by conan
    set_property(TARGET audresample PROPERTY POSITION_INDEPENDENT_CODE ${CMAKE_POSITION_INDEPENDENT_CODE})
else()
    set_property(TARGET audresample PROPERTY POSITION_INDEPENDENT_CODE ON) # enable by default
endif()


target_include_directories(audresample
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

find_library(soxr soxr)
target_link_libraries(audresample PRIVATE ${soxr})


#### EXTRA TOOLS ######

if(BUILD_TOOLS)

	find_package(audeering REQUIRED)
	
	## oneshot converter ##
	add_executable(src_oneshot src_oneshot.c)
	target_include_directories(src_oneshot 
		PRIVATE
			"${CMAKE_CURRENT_SOURCE_DIR}/include"
	)
	
	target_link_libraries(src_oneshot PRIVATE audeering)
	target_link_libraries(src_oneshot PRIVATE audresample)
	
	## streaming converter ##
	add_executable(src_streaming src_streaming.c)
	
	target_include_directories(src_streaming
		PRIVATE
			"${CMAKE_CURRENT_SOURCE_DIR}/include"
	)
	target_link_libraries(src_streaming PRIVATE audeering)
	target_link_libraries(src_streaming PRIVATE audresample)
endif()
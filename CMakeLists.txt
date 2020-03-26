cmake_minimum_required(VERSION 3.5.1)

project(AUDRESAMPLE)

option(BUILD_SHARED_LIB "Build libaudresample as a shared object." OFF)
set(AUDEERING_LIBS_PATH "" CACHE PATH "Path to the audeering libs source folder.")

# set C standard to version 99 for all targets
set(CMAKE_C_STANDARD 99)

# List of source files: libaudresample_SOURCES
set(libaudresample_SOURCES
    src/audresample.c
)

if(BUILD_SHARED_LIB)
    add_library(audresample SHARED ${libaudresample_SOURCES})
else()
    add_library(audresample STATIC ${libaudresample_SOURCES})
endif()

# allow linking of libaudresample into shared libraries
set_property(TARGET audresample PROPERTY POSITION_INDEPENDENT_CODE ON)

target_include_directories(audresample
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
		"${AUDEERING_LIBS_PATH}/include"
)

find_library(soxr soxr)
target_link_libraries(audresample PRIVATE soxr)


###### executable (oneshot) ######

add_executable(src_oneshot src_oneshot.c)
target_include_directories(src_oneshot 
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
		"${AUDEERING_LIBS_PATH}/include"
)

find_package(audeering REQUIRED)
target_link_libraries(src_oneshot PRIVATE audeering)
target_link_libraries(src_oneshot PRIVATE audresample)


###### executable (streaming) ######

add_executable(src_streaming src_streaming.c)

target_include_directories(src_streaming
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
		"${AUDEERING_LIBS_PATH}/include"
)
target_link_libraries(src_streaming PRIVATE audeering)
target_link_libraries(src_streaming PRIVATE audresample)
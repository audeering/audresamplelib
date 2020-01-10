cmake_minimum_required(VERSION 3.5.1)

project (sr-converter)

add_executable(sr-converter sr-converter.c)

include_directories(${INCLUDE_DIRECTORIES})

find_library(audeering audeering ${CMAKE_LIBRARY_PATH})
target_link_libraries(sr-converter PRIVATE audeering)

find_library(soxr soxr ${CMAKE_LIBRARY_PATH})
target_link_libraries(sr-converter PRIVATE soxr)
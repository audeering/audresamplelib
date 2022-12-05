cmake_minimum_required(VERSION 3.10.2)
project(audresample-tests CXX)

# set C++ standard to version 11 for all targets
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message(FATAL_ERROR "Tests cannot run without conan!")
endif()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(KEEP_RPATHS)

# workaround the fact that "conan test" does not run packaging steps
# that would allows us to copy the test resources via Conan
get_filename_component(ABSOLUTE_PATH_TO_RES "test-assets" ABSOLUTE)
if(EXISTS "${ABSOLUTE_PATH_TO_RES}")
    file(COPY "test-assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

add_executable(audresample-test
    src/main.cpp
    src/resample.cpp
)

target_link_libraries(audresample-test audresample ${CONAN_LIBS})

target_include_directories(audresample-test PRIVATE ${CONAN_INCLUDE_DIRS})

enable_testing()

add_test(
    NAME audresample-test
    COMMAND audresample-test
)

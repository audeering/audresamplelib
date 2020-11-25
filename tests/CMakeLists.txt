cmake_minimum_required(VERSION 3.10.2)
project(devaice-sdk-base-tests CXX)

add_executable(audresample-test main.cpp resample.cpp)

target_link_libraries(audresample-test audresample ${CONAN_LIBS})

target_include_directories(
    audresample-test PRIVATE ../include ${CONAN_INCLUDE_DIRS}
)
         
add_test(
    NAME audresample-test
    COMMAND audresample-test
)
         
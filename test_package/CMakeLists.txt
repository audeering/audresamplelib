cmake_minimum_required(VERSION 3.10.2)
project(audresample-tests CXX)

# workaround the fact that "conan test" does not run packaging steps
# that would allows us to copy the test resources via Conan
get_filename_component(ABSOLUTE_PATH_TO_RES "test-assets" ABSOLUTE)
if(EXISTS "${ABSOLUTE_PATH_TO_RES}")
    file(COPY "test-assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

find_package(audresamplelib REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(audresample-test src/resample.cpp)
target_link_libraries(audresample-test 
    PRIVATE 
        audresamplelib::audresamplelib
        Catch2::Catch2WithMain
)

enable_testing()
add_test(NAME audresample-test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND audresample-test)

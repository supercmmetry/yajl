cmake_minimum_required(VERSION 3.16)
project(yajl)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_subdirectory(other)

add_library(yajl STATIC
        src/yajl.h src/markers.h)

target_compile_features(yajl PUBLIC cxx_std_20)
set_target_properties(yajl PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(yajl PUBLIC bitio)


# define tests
add_executable(zyajl_demo tests/demo.cpp)
target_link_libraries(zyajl_demo PUBLIC yajl)

add_test(yajl_demo zyajl_demo)
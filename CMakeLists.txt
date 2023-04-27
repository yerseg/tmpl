cmake_minimum_required(VERSION 3.21)

project(tmpl VERSION 0.0.1)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

add_subdirectory(contrib)
add_subdirectory(test)

add_library(tmpl INTERFACE)

target_include_directories(tmpl INTERFACE include)

target_link_libraries(tmpl INTERFACE Boost::mp11 Boost::preprocessor)

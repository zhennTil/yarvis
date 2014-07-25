
cmake_minimum_required (VERSION 2.6)
project (YARVIS)

set (YARVIS_LIB_DIR "${CMAKE_SOURCE_DIR}/lib")

list (APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory (yarvis-server)

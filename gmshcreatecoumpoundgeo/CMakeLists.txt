project(gmshcreatecompoundgeo)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
include_directories(/usr/local_machine/gmsh-2.9.3/include/gmsh)
link_directories(/usr/local_machine/gmsh-2.9.3/lib)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} Gmsh)
list(APPEND CMAKE_CXX_FLAGS "-Wall -pedantic -O3 -std=c++11" ${CMAKE_CXX_FLAGS})

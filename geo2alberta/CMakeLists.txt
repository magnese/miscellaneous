cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
include_directories(/usr/local_machine/gmsh-2.9.3/include/gmsh)
link_directories(/usr/local_machine/gmsh-2.9.3/lib)
add_executable(geo2alberta2D geo2alberta2D.cpp)
target_link_libraries(geo2alberta2D Gmsh)
add_executable(geo2alberta3D geo2alberta3D.cpp)
target_link_libraries(geo2alberta3D Gmsh)
add_executable(msh2alberta msh2alberta.cpp)
list(APPEND CMAKE_CXX_FLAGS "-Wall -pedantic -O3 -std=c++11" ${CMAKE_CXX_FLAGS})

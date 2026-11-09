cmake_minimum_required(VERSION 3.9)
project(GameOfLife CXX)

find_package(OpenMP REQUIRED)

add_executable(parallel-matrix src/parallel-matrix.cpp)
add_executable(serial-matrix src/serial-matrix.cpp)
add_executable(parallel-set src/parallel-set.cpp)
add_executable(serial-set src/serial-set.cpp)
add_executable(serial-vecmatrix src/serial-vecmatrix.cpp)

if(OpenMP_CXX_FOUND)
    target_link_libraries(parallel-matrix PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(serial-matrix PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(parallel-set PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(serial-set PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(serial-vecmatrix PUBLIC OpenMP::OpenMP_CXX)
    
endif()
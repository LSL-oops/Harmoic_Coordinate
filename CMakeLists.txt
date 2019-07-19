cmake_minimum_required(VERSION 3.14)
project(HarmonicCoordinateDemo)

set(CMAKE_CXX_STANDARD 14)

find_package(CUDA)
if(${CUDA_FOUND})
    include_directories(${CUDA_INCLUDE_DIRS})
    link_directories($ENV{CUDA_PATH}/lib/x64)
else(${CUDA_FOUND})
    MESSAGE(STATUS "cuda not found")
endif(${CUDA_FOUND})

set(INCLUDE_DIR /usr/local/cuda-10.1/targets/x86_64-linux/include/)
set(LIB_DIR /usr/lib/x86_64-linux-gnu/)

set_source_files_properties(kernel.cu PROPERTIES CUDA_SOURCE_PROPERTY_FORMAT OBJ)
file(GLOB_RECURSE CURRENT_HEADERS *.h *.hpp *.cuh)
file(GLOB CURRENT_SOURCES *.cpp *.cu *.cc)
source_group("Include" FILES ${CURRENT_HEADERS})
source_group("Source" FILES ${CURRENT_SOURCES})

include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})

cuda_add_executable(HarmonicCoordinateDemo main.cpp kernel.cu glad.c)
target_link_libraries(HarmonicCoordinateDemo glfw dl)
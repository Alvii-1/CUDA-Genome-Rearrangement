# REMEMBER TO CHANGE <project> TO THE NAME OF THE NEW PROJECT

# --- | Setting minimum version
cmake_minimum_required(VERSION 3.20)

# --- | Project name (Change name for each project)
project(project LANGUAGES CXX CUDA)

# --- | Set C++ and CUDA Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# --- | Output exe in build/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Collect sources
file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
file(GLOB_RECURSE CUDA_SOURCES "kernels/*.cu")
set(ALL_SOURCES ${CPP_SOURCES} ${CUDA_SOURCES})

# --- | Add executable from all sources
add_executable(project ${ALL_SOURCES})

# --- | Required for hybrid linking below
find_package(CUDAToolkit REQUIRED)
target_link_libraries(project PRIVATE CUDA::cudart)

# --- | Include directories
target_include_directories(project PRIVATE include)

# --- | CUDA properties
set_target_properties(project PROPERTIES
    CUDA_SEPARABLE_COMPILATION OFF
    CUDA_ARCHITECTURES 89
    CUDA_RUNTIME_LIBRARY hybrid
)

# --- | Force all code to use MD (release) or MDd (debug) runtime
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CUDA_FLAGS_DEBUG "${CMAKE_CUDA_FLAGS_DEBUG} -Xcompiler /MDd")
    set(CMAKE_CUDA_FLAGS_RELEASE "${CMAKE_CUDA_FLAGS_RELEASE} -Xcompiler /MD")
endif()
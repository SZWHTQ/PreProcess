cmake_minimum_required(VERSION 3.12)
project(
    PreProcess
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set OpenCASCADE directory (change this to the actual path on your system)
if(WIN32)
    set(CMAKE_PREFIX_PATH "E:/Repository/OpenCASCADE/7.7.0/opencascade-7.7.0/cmake")
else()
    set(CMAKE_PREFIX_PATH "~/Repository/OpenCASCADE/latest/lib/cmake/opencascade")
endif()

# Find OpenCASCADE
find_package(OpenCASCADE REQUIRED)
find_package(OpenMP)

# Add your source files
set(srcs)
add_subdirectory("src")
include_directories("include")

# Create the executable
add_library(${PROJECT_NAME} ${srcs})

add_executable("PreProcessor" "src/main.cc")
target_link_libraries("PreProcessor" ${PROJECT_NAME})

if(OpenCASCADE_FOUND)
    message(STATUS "OpenCASCADE found: version ${OpenCASCADE_VERSION}")
    include_directories(${OpenCASCADE_INCLUDE_DIR})
    # Link with OpenCASCADE libraries
    target_link_libraries(${PROJECT_NAME} ${OpenCASCADE_LIBRARIES})
else()
    message(FATAL_ERROR "OpenCASCADE not found")
endif()

if(OpenMP_CXX_FOUND)
    # message(STATUS "OpenMP found: version ${OpenMP_CXX_VERSION}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
else()
    message(STATUS "OpenMP not found")
    
endif()
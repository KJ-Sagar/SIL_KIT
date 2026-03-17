cmake_minimum_required(VERSION 3.5)
 
Project(RestBus)
set(CMAKE_BUILD_TYPE Debug)
 
# CMake build configuration required is exported to SilKit/lib/cmake/SilKit and defines SilKit::SilKit target 
# From CMake this can be consumed via the find package command
find_package(SilKit 
    REQUIRED CONFIG
    PATHS "${CMAKE_CURRENT_LIST_DIR}/SilKit"
)
 
find_package(Threads REQUIRED)

# Use target_include_directories for better scoping

 
add_executable(${PROJECT_NAME} RestBus.cpp)

 
target_link_libraries(${PROJECT_NAME} SilKit::SilKit Threads::Threads)





 


 


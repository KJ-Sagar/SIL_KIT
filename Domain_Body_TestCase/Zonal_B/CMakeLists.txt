cmake_minimum_required(VERSION 3.5)
 
Project(rest_bus_app)
set(CMAKE_BUILD_TYPE Debug)
 
# CMake build configuration required is exported to SilKit/lib/cmake/SilKit and defines SilKit::SilKit target 
# From CMake this can be consumed via the find package command
find_package(SilKit 
    REQUIRED CONFIG
    PATHS "${CMAKE_CURRENT_LIST_DIR}/SilKit"
)
 
find_package(Threads REQUIRED)

# Use target_include_directories for better scoping

 
add_executable(Zonal2_app src/ZonalB.cpp)

 
target_link_libraries(Zonal2_app SilKit::SilKit Threads::Threads)




 


 


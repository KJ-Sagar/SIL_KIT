cmake_minimum_required(VERSION 3.5)
 
Project(ZonalA)

set(CMAKE_BUILD_TYPE Debug) 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_FLAGS_DEBUG "-g")

find_package(SilKit 
    REQUIRED CONFIG
    PATHS "${CMAKE_CURRENT_LIST_DIR}/SilKit"
)

find_package(Threads REQUIRED)
 
add_executable(${PROJECT_NAME} ZonalA.cpp) 
target_link_libraries(${PROJECT_NAME} PRIVATE SilKit::SilKit Threads::Threads)
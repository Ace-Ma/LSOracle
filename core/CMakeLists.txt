cmake_minimum_required (VERSION 3.9)                  # CMake minimum version
project(lsoracle)

include_directories(${PROJECT_SOURCE_DIR}/lib/kahypar/include)  # add OpenTimer include
#include_directories(${PROJECT_SOURCE_DIR}/lib/darknet/include)

include_directories(${PROJECT_SOURCE_DIR}/../lib/oracle/include/oracle/classification/fplus/include)
include_directories(${PROJECT_SOURCE_DIR}/../lib/oracle/include/oracle/classification/eigen)
include_directories(${PROJECT_SOURCE_DIR}/../lib/oracle/include/oracle/classification/json/include)
include_directories(${PROJECT_SOURCE_DIR}/../lib/oracle/include/oracle/classification/fdeep_keras/include)

set(CMAKE_CXX_STANDARD 17)                            # enable c++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)                        # thread library (pthread)

add_executable(lsoracle lsoracle.cpp)

target_include_directories(lsoracle PRIVATE ../lib/kahypar/include)
#target_include_directories(lsoracle PRIVATE ../lib/darknet/include)
#darklib removed
target_link_libraries(lsoracle oracle mockturtle kitty alice libabc OpenTimer Threads::Threads stdc++fs kahypar)

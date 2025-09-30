cmake_minimum_required(VERSION 3.31)
project(test3)

set(CMAKE_CXX_STANDARD 20)

# Set output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lua-5.4.7 ${CMAKE_BINARY_DIR}/lua EXCLUDE_FROM_ALL)

add_executable(test3 main.cpp funcs.cpp)
target_link_libraries(test3 PRIVATE lua_cpp)
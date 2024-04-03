cmake_minimum_required(VERSION 3.25)
project(server)

# Our Project
add_executable(${PROJECT_NAME} main.cpp server.cpp)
target_link_libraries(${PROJECT_NAME} utils)

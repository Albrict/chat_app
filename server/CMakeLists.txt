cmake_minimum_required(VERSION 3.25)
project(server)

# Our Project

file(GLOB_RECURSE cpp_sources CONFIGURE_DEPENDS "*.cpp")
file(GLOB_RECURSE c_sources CONFIGURE_DEPENDS "*.c")

add_executable(${PROJECT_NAME} ${cpp_sources} ${c_sources})
target_link_libraries(${PROJECT_NAME} utils)

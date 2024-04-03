cmake_minimum_required( VERSION 3.25.0 )
project(chat_system)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")
set (CMAKE_CXX_STANDARD 20)

add_subdirectory("utils")
add_subdirectory("server")
add_subdirectory("client")

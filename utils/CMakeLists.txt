cmake_minimum_required(VERSION 3.25)
project(utils)

include(GNUInstallDirs)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp")
add_library(${PROJECT_NAME} STATIC ${sources})
target_include_directories(${PROJECT_NAME} PRIVATE .)

install(TARGETS ${PROJECT_NAME} 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

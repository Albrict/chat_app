cmake_minimum_required(VERSION 3.25)
project(client)

cmake_policy(SET CMP0004 OLD)
cmake_policy(SET CMP0004 OLD)

execute_process(
    COMMAND bash -c "fltk-config --cxxflags"
    OUTPUT_VARIABLE cxx_flags_var
)

execute_process(
    COMMAND bash -c "fltk-config --ldflags"
    OUTPUT_VARIABLE ld_flags_var
)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp")

# Our Project
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} ${ld_flags_var})
add_compile_options(${PROJECT_NAME} "${${cxx_flags_var}}")

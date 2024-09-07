cmake_minimum_required(VERSION 3.12)
project(penis)
add_subdirectory(extern/glfw-3.4)

add_executable(${PROJECT_NAME}
    src/main.cc
    src/gl.c

)
include_directories(include/)
target_link_libraries(${PROJECT_NAME} glfw)
link_libraries(m)
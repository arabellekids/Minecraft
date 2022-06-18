cmake_minimum_required(VERSION 3.0)

project(Crafti)

include(ProjFiles.txt)

add_executable(${PROJECT_NAME} ${FILES})

find_library(SDL2 NAMES libSDL2.so REQUIRED)
find_library(OPENGL NAMES libOpenGL.so REQUIRED)

target_link_libraries(${PROJECT_NAME} ${SDL2})
target_link_libraries(${PROJECT_NAME} ${OPENGL})
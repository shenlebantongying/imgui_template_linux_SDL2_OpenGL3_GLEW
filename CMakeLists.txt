cmake_minimum_required(VERSION 3.17)
project(uDataShore)
set(CMAKE_CXX_STANDARD 17)

set(IMGUI_DIR ./imgui/)
include_directories()

find_package(OpenGL REQUIRED)
add_subdirectory(glad)
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS} ${IMGUI_DIR} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

file(GLOB sources ./imgui/*.cpp)

add_executable(uDataShore main.cpp ${sources}
        ${IMGUI_DIR}/imgui_impl_sdl.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.cpp)

target_link_libraries(uDataShore ${SDL2_LIBRARIES}  OpenGL::GL glad ${CMAKE_DL_LIBS})

FILE(COPY Roboto-Medium.ttf DESTINATION "${CMAKE_BINARY_DIR}")
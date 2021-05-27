cmake_minimum_required(VERSION 3.16.3)
project(CASCADED_SHADOW_MAPS)

find_package ( SDL2 REQUIRED )
find_package ( glm REQUIRED )
find_package ( GLEW REQUIRED )
find_package ( OpenGL REQUIRED )
find_package(glfw3 3.3 REQUIRED)


add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set ( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set ( CMAKE_BUILD_TYPE DEBUG )
set(CMAKE_CXX_STANDARD 17)

include_directories(external_include)
link_directories(external_include)

add_executable(CASCADED_SHADOW_MAPS
        main.cpp
        external_include/glad.c
        camera.h
        shader.h
        stb_image.h)

target_link_libraries(CASCADED_SHADOW_MAPS OpenGL::GL glfw ${CMAKE_DL_LIBS})
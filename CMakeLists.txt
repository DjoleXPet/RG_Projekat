cmake_minimum_required(VERSION 3.20)
project(RG_projekat)

 set(CMAKE_CXX_STANDARD 17 )

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(ASSIMP REQUIRED)



include_directories(include/)

add_subdirectory(external/glad)
add_executable(RG_projekat main.cpp stb_image.cpp include/Camera.h include/Mesh.h include/Model.h include/Lampa.h)
target_link_libraries(RG_projekat glad glfw ${CMAKE_DL_LIBS} assimp)


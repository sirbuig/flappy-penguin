cmake_minimum_required(VERSION 3.29)
project(flappy_penguin)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(main src/main.cpp ../src/loadShaders.cpp)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(main OpenGL::GL GLUT::GLUT GLEW::GLEW GLU SOIL)

set(SHADER_FILES
    assets/shaders/example.frag
    assets/shaders/example.vert
)

set(IMG_FILES
    assets/textures/pinguin.png
)

file(COPY ${SHADER_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${IMG_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
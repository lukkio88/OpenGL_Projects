cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(OglPlaygroud)
set(CMAKE_CXX_STANDARD 11)

#find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)

set(SRC main.cpp)
set(HEADER )
link_directories(./dependencies/lib-vc2015)
set(GLFW_LIB glfw3)

add_executable(OglPlayground ${SRC} ${HEADER})
target_include_directories(OglPlayground PUBLIC ./dependencies/include ${OPENGL_INCLUDE_DIR})
target_link_libraries(OglPlayground ${GLFW_LIB} ${OPENGL_gl_LIBRARY})
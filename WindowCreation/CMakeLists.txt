cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(OpenGLTutorial)

set(CMAKE_CXX_STANDARD 14)
set(SOURCE_FILES ${CMAKE_SOURCE_DIR}/main.cpp)
set(DEPENDENCIES ${CMAKE_SOURCE_DIR}/dependencies)

add_library(glfw STATIC IMPORTED)
add_library(opengl STATIC IMPORTED)
set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${DEPENDENCIES}/GLFW/lib/glfw3.lib)
set_target_properties(opengl PROPERTIES IMPORTED_LOCATION ${DEPENDENCIES}/OpenGL/lib/OpenGL32.Lib)

add_executable(WindowCreation_bin ${SOURCE_FILES} ${DEPENDENCIES}/glad/src/glad.c)
target_include_directories(WindowCreation_bin PUBLIC 
	${DEPENDENCIES}/GLFW/ince
	${DEPENDENCIES}/glad/includ
target_link_libraries(WindowCreation_bin glfw opengl)
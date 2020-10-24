cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(OpenGLTutorial)

set(CMAKE_CXX_STANDARD 14)

set(DEPENDENCIES ${CMAKE_SOURCE_DIR}/dependencies)
add_library(glfw STATIC IMPORTED)
add_library(opengl STATIC IMPORTED)
set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${DEPENDENCIES}/GLFW/lib/glfw3.lib)
set_target_properties(opengl PROPERTIES IMPORTED_LOCATION ${DEPENDENCIES}/OpenGL/lib/OpenGL32.Lib)

set(PROJECTS WindowCreation FirstTriangle Shaders)
foreach(project_name ${PROJECTS})
	set(SOURCE_FILES ${CMAKE_SOURCE_DIR}/${project_name}/main.cpp)
	add_executable(${project_name}_bin ${SOURCE_FILES} ${DEPENDENCIES}/glad/src/glad.c)
	target_include_directories(${project_name}_bin PUBLIC 
		${DEPENDENCIES}/GLFW/include
		${DEPENDENCIES}/glad/include)
	target_link_libraries(${project_name}_bin glfw opengl)
endforeach()
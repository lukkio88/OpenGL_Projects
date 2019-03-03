#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using std::ifstream;
using std::stringstream;
using std::getline;
using std::string;

struct ShaderProgramsSrcs {
	string vertex_shader;
	string fragment_shader;
};

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x,__FILE__,__LINE__))

bool GLLogCall(const char * function, const char * file, int line);
void GLClearError();

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

ShaderProgramsSrcs readShaderSrcs(const string& filename);
unsigned int compileShader(unsigned int type, const string& src);
unsigned int createShader(const ShaderProgramsSrcs& src);
#include <header.h>

bool GLLogCall(const char * function, const char * file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] " << "): " << function <<
			" " << file << ": " << line << std::endl;
		return false;
	}
	return true;
}

void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
	__debugbreak();
}
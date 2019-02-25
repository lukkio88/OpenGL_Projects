#include <header.h>

bool GLLogCall(const char * function, const char * file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] " << "): " << function <<
			" " << file << ": " << line << std::endl;
		return false;
	}
	return true;
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

ShaderProgramsSrcs readShaderSrcs(const string& filename) {
	ifstream ifs(filename);
	string curr_line;
	stringstream ss[2];
	int idx = -1;
	const size_t not_found = std::string::npos;

	while (getline(ifs, curr_line)) {
		if (curr_line.find("#shader") != not_found) {
			if (curr_line.find("vertex") != not_found)
				idx = 0;
			else if (curr_line.find("fragment") != not_found)
				idx = 1;
		}
		else {
			ss[idx] << curr_line << std::endl;
		}
	}

	return ShaderProgramsSrcs{ ss[0].str(),ss[1].str() };

}

unsigned int compileShader(unsigned int type, const string& src) {
	unsigned int id = glCreateShader(type);
	const char * _src = src.c_str();
	glShaderSource(id, 1, &_src, nullptr);
	glCompileShader(id);

	int result_compile_status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result_compile_status);

	if (result_compile_status == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *log_msg = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, log_msg);
		std::cout << "Compilation failed : " << std::endl;
		std::cout << log_msg << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int createShader(const ShaderProgramsSrcs& src) {
	unsigned int program = glCreateProgram();
	unsigned int vs_id = compileShader(GL_VERTEX_SHADER, src.vertex_shader);
	unsigned int fs_id = compileShader(GL_FRAGMENT_SHADER, src.fragment_shader);

	glAttachShader(program, vs_id);
	glAttachShader(program, fs_id);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs_id);
	glDeleteShader(fs_id);

	return program;
}
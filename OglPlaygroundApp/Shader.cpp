#include <Shader.h>

Shader::Shader(const std::string & path):
	m_FilePath(path),m_RendererID(0)
{
	ShaderProgramsSrcs programsSrc = readShaderSrcs(m_FilePath);
	m_RendererID = CreateShader(programsSrc);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));

}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string & name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), 0.0, 1.0, 0.0, 1.0));
}

ShaderProgramsSrcs Shader::readShaderSrcs(const string& filename) {
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

unsigned int Shader::CompileShader(unsigned int type, const string& src) {
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

unsigned int Shader::CreateShader(const ShaderProgramsSrcs& src) {
	unsigned int program = glCreateProgram();
	unsigned int vs_id = CompileShader(GL_VERTEX_SHADER, src.vertex_shader);
	unsigned int fs_id = CompileShader(GL_FRAGMENT_SHADER, src.fragment_shader);

	glAttachShader(program, vs_id);
	glAttachShader(program, fs_id);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs_id);
	glDeleteShader(fs_id);

	return program;
}

unsigned int Shader::GetUniformLocation(const std::string & name)
{
	if (m_UniformdLocationCache.find(name) != m_UniformdLocationCache.end())
		return m_UniformdLocationCache[name];
	GLCall(unsigned int uniform_idx = glGetUniformLocation(m_RendererID, name.c_str()));
	if (uniform_idx == -1)
		std::cout << "Warning: uniform" << name << " doesn't exist!" << std::endl;
	m_UniformdLocationCache[name] = uniform_idx;
	return uniform_idx;
}
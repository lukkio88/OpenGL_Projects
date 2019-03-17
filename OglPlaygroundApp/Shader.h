#pragma once

#include <Common.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct ShaderProgramsSrcs {
	string vertex_shader;
	string fragment_shader;
};

class Shader {
public:
	Shader(const std::string& path);
	~Shader();

	void Bind() const;
	void Unbind() const;

	//set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name,const glm::mat4& matrix);
private:
	ShaderProgramsSrcs readShaderSrcs(const string& filename);
	unsigned int CompileShader(unsigned int type, const string& src);
	unsigned int CreateShader(const ShaderProgramsSrcs& src);
	int GetUniformLocation(const std::string& name);

private:
	std::unordered_map<std::string,int> m_UniformdLocationCache;
	std::string m_FilePath;
	unsigned int m_RendererID;
	//caching for uniform
};
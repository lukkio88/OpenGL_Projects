#pragma once

#include <Common.h>
#include <string>
#include <unordered_map>

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
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
	ShaderProgramsSrcs readShaderSrcs(const string& filename);
	unsigned int CompileShader(unsigned int type, const string& src);
	unsigned int CreateShader(const ShaderProgramsSrcs& src);
	unsigned int GetUniformLocation(const std::string& name);

private:
	std::unordered_map<std::string, unsigned int> m_UniformdLocationCache;
	std::string m_FilePath;
	unsigned int m_RendererID;
	//caching for uniform
};
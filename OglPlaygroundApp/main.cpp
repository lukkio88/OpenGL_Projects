#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using std::ifstream;
using std::stringstream;
using std::getline;
using std::string;

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr,"GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
	//__debugbreak();
}

struct ShaderProgramsSrcs {
	string vertex_shader;
	string fragment_shader;
};

static ShaderProgramsSrcs readShaderSrcs(const string& filename) {
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

static unsigned int compileShader(unsigned int type, const string& src) {
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

static unsigned int createShader(const ShaderProgramsSrcs& src) {
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

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	
	/*Declaring the debug context*/
	//if (glfwExtensionSupported("GL_ARB_debug_output") == GLFW_TRUE) {
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	//}

	if (glewInit() != GLEW_OK) {
		std::cout << "Error!" << std::endl;
	}

	// Set debug callback
	/*if (glDebugMessageCallback) {
		glDebugMessageCallback(MessageCallback, 0);
	}
	glEnable(GL_DEBUG_OUTPUT);*/


	std::cout << glGetString(GL_VERSION) << std::endl;

	float position[8] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f,0.5f
	};

	unsigned int idx[6]{
		0,1,2,2,3,0
	};

	unsigned int position_buffer;
	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float),position,GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float),0);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), idx, GL_STATIC_DRAW);

	ShaderProgramsSrcs programsSrc = readShaderSrcs("../Basic.shader");
	unsigned int program = createShader(programsSrc);
	glUseProgram(program);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

		//glDrawArrays(GL_TRIANGLES, 0, 3);
		int uniform_idx = glGetUniformLocation(program, "u_Color");
		glUniform4f(uniform_idx, 0.0, 1.0, 0.0, 1.0);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); //Why the nullptr?


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

	glDeleteProgram(program);

    glfwTerminate();
    return 0;
}
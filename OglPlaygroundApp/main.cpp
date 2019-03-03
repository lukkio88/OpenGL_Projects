#include <header.h>
#include <VertexBuffer.h>
#include <IndexBuffer.h>
#include <VertexArray.h>

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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Set debug callback
	/*if (glDebugMessageCallback) {
		glDebugMessageCallback(MessageCallback, 0);
	}*/
	glEnable(GL_DEBUG_OUTPUT);


	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		float position[8] = {
			-0.5f, -0.5f,
			0.5f, -0.5f,
			0.5f, 0.5f,
			-0.5f,0.5f
		};

		unsigned int idx[6]{
			0,1,2,2,3,0
		};

		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		VertexArray va;
		VertexBuffer vb(position, 8 * sizeof(float));
		VertexBufferLayout layout;

		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(idx, 6);

		ShaderProgramsSrcs programsSrc = readShaderSrcs("../Basic.shader");
		unsigned int program = createShader(programsSrc);
		glUseProgram(program);

		int uniform_idx = glGetUniformLocation(program, "u_Color");
		glUniform4f(uniform_idx, 0.0, 1.0, 0.0, 1.0);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			glClear(GL_COLOR_BUFFER_BIT);

			va.Bind();
			ib.Bind();

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		glDeleteProgram(program);
	}

	glfwTerminate();
	return 0;
}
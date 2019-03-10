#include <Texture.h>
#include <Windows.h>

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
		float position[] = {
			-0.5f, -0.5f, 0.0f, 0.0f,
			0.5f, -0.5f, 1.0f, 0.0f,
			0.5f, 0.5f, 1.0f, 1.0f,
			-0.5f,0.5f, 0.0f, 1.0f
		};

		unsigned int idx[6]{
			0,1,2,
			2,3,0
		};

		GLCall(glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glEnable(GL_BLEND));

		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		VertexArray va;
		VertexBuffer vb(position, 16 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(idx, 6);

		glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

		Shader shader("./Basic.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.0f, 0.3f, 0.8f, 1.0f);
		shader.SetUniformMat4f("u_MVP", proj);

		Texture texture("layla.png");
		texture.Bind();
		shader.SetUniform1i("u_Texture", 0);

		va.Unbind();
		shader.Unbind();
		vb.Unbind();
		ib.Unbind();

		Renderer renderer;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{

			renderer.Clear();

			shader.Bind();
			shader.SetUniform4f("u_Color", 0.0f, 0.3f, 0.8f, 1.0f);

			renderer.Draw(va, ib, shader);


			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

	}

	glfwTerminate();
	return 0;
}
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

const char* vertexShaderSource = //same for light
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"	FragPos = vec3(model * vec4(aPos, 1.0));\n"
"	Normal = aNormal;\n"
"	gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"}\n";

const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 objectColor;\n"
"void main()\n"
"{\n"
"	float ambientStrength = 0.1;\n" //ambient light
"	vec3 ambient = ambientStrength * lightColor;\n"
"	vec3 norm = normalize(Normal);\n" //diffuse
"	vec3 lightDir = normalize(lightPos - FragPos);\n"
"	float diff = max(dot(norm, lightDir), 0.0);\n"
"	vec3 diffuse = diff * lightColor;\n"
"	vec3 result = (ambient + diffuse) * objectColor;\n"
"	FragColor = vec4(result, 1.0);\n"
"}\n";

const char* vertexShaderLightSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

const char* fragmentShaderLightSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0);\n"
"}\n";

const char* vertexShaderError = "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n";
const char* fragmentShaderError = "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n";
const char* shaderProgramError = "ERROR::SHADER::PROGRAM::LINKING_FAILED\n";

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

bool checkShaderError(int success, int shaderId, const char* shaderError)
{
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cout << shaderError <<
			infoLog << std::endl;
	}
	return success;
}

//As we require two shaders here I define some utility functions, a class would be better for so many reasons
//but I'll stick with functions now
int createAndCompileShader(const char* shaderSourceCode, unsigned int& shaderId, unsigned int shaderType)
{
	shaderId = glCreateShader(shaderType);

	glShaderSource(shaderId, 1, &shaderSourceCode, NULL);
	glCompileShader(shaderId);

	int success;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	return success;
}

int createAndLinkShaderProgram(unsigned int vertexShaderId, unsigned int fragmentShaderId, unsigned int& shaderProgram)
{
	shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShaderId);
	glAttachShader(shaderProgram, fragmentShaderId);
	glLinkProgram(shaderProgram);

	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	return success;
}


int mainDiffuse()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEPTH_TEST); //TODO: why doesn't work?

	//Shader section
	unsigned int vertexShader = 0, vertexShaderLight = 0;
	checkShaderError(
		createAndCompileShader(vertexShaderSource, vertexShader, GL_VERTEX_SHADER), 
		vertexShader, 
		vertexShaderError
	);

	checkShaderError(
		createAndCompileShader(vertexShaderLightSource, vertexShaderLight, GL_VERTEX_SHADER),
		vertexShaderLight,
		vertexShaderError
	);

	unsigned int fragmentShader = 0, fragmentShaderLight = 0;
	checkShaderError(
		createAndCompileShader(fragmentShaderSource, fragmentShader, GL_FRAGMENT_SHADER),
		fragmentShader,
		fragmentShaderError
	);

	checkShaderError(
		createAndCompileShader(fragmentShaderLightSource, fragmentShaderLight, GL_FRAGMENT_SHADER),
		fragmentShaderLight,
		fragmentShaderError
	);

	unsigned int shaderProgram = 0, shaderProgramLight = 0;
	checkShaderError(
		createAndLinkShaderProgram(vertexShader, fragmentShader, shaderProgram),
		shaderProgram,
		shaderProgramError
	);

	checkShaderError(
		createAndLinkShaderProgram(vertexShaderLight, fragmentShaderLight, shaderProgramLight),
		shaderProgramLight,
		shaderProgramError
	);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShaderLight);
	glDeleteShader(fragmentShaderLight);

	glUseProgram(shaderProgram); //Not sure what to do with this

	//Buffer section
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// lighting position
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(shaderProgram);

		//Light settings
		glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		GLuint objectColorLocation = glGetUniformLocation(shaderProgram, "objectColor");
		GLuint lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor");
		GLuint lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos");
		glUniform3fv(objectColorLocation, 1, glm::value_ptr(objectColor));
		glUniform3fv(lightColorLocation, 1, glm::value_ptr(lightColor));
		glUniform3fv(lightPosLocation, 1, glm::value_ptr(lightPos));

		// camera/view transformation
		GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "view");
		GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projection");
		GLuint modelMatrixLocation = glGetUniformLocation(shaderProgram, "model");


		glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 model = glm::mat4(1.0f);
		float angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(model));

		// render the cube
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// also draw the lamp object
		glUseProgram(shaderProgramLight);
		projectionMatrixLocation = glGetUniformLocation(shaderProgramLight, "projection");
		viewMatrixLocation = glGetUniformLocation(shaderProgramLight,"view");
		modelMatrixLocation = glGetUniformLocation(shaderProgramLight, "model");
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
		
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;

}

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Shader section
	unsigned int vertexShader = 0, vertexShaderLight = 0;
	checkShaderError(
		createAndCompileShader(vertexShaderSource, vertexShader, GL_VERTEX_SHADER),
		vertexShader,
		vertexShaderError
	);

	checkShaderError(
		createAndCompileShader(vertexShaderLightSource, vertexShaderLight, GL_VERTEX_SHADER),
		vertexShaderLight,
		vertexShaderError
	);

	unsigned int fragmentShader = 0, fragmentShaderLight = 0;
	checkShaderError(
		createAndCompileShader(fragmentShaderSource, fragmentShader, GL_FRAGMENT_SHADER),
		fragmentShader,
		fragmentShaderError
	);

	checkShaderError(
		createAndCompileShader(fragmentShaderLightSource, fragmentShaderLight, GL_FRAGMENT_SHADER),
		fragmentShaderLight,
		fragmentShaderError
	);

	unsigned int shaderProgram = 0, shaderProgramLight = 0;
	checkShaderError(
		createAndLinkShaderProgram(vertexShader, fragmentShader, shaderProgram),
		shaderProgram,
		shaderProgramError
	);

	checkShaderError(
		createAndLinkShaderProgram(vertexShaderLight, fragmentShaderLight, shaderProgramLight),
		shaderProgramLight,
		shaderProgramError
	);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShaderLight);
	glDeleteShader(fragmentShaderLight);

	glUseProgram(shaderProgram); //Not sure what to do with this

	//Buffer section
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// lighting position
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(shaderProgram);

		//Light settings
		glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		GLuint objectColorLocation = glGetUniformLocation(shaderProgram, "objectColor");
		GLuint lightColorLocation = glGetUniformLocation(shaderProgram, "lightColor");
		GLuint lightPosLocation = glGetUniformLocation(shaderProgram, "lightPos");
		glUniform3fv(objectColorLocation, 1, glm::value_ptr(objectColor));
		glUniform3fv(lightColorLocation, 1, glm::value_ptr(lightColor));
		glUniform3fv(lightPosLocation, 1, glm::value_ptr(lightPos));

		// camera/view transformation
		GLuint viewMatrixLocation = glGetUniformLocation(shaderProgram, "view");
		GLuint projectionMatrixLocation = glGetUniformLocation(shaderProgram, "projection");
		GLuint modelMatrixLocation = glGetUniformLocation(shaderProgram, "model");


		glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		glm::mat4 model = glm::mat4(1.0f);
		float angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(model));

		// render the cube
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// also draw the lamp object
		glUseProgram(shaderProgramLight);
		projectionMatrixLocation = glGetUniformLocation(shaderProgramLight, "projection");
		viewMatrixLocation = glGetUniformLocation(shaderProgramLight, "view");
		modelMatrixLocation = glGetUniformLocation(shaderProgramLight, "model");
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube

		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;

}
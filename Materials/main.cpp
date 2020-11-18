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

const char* lightCubeVertexShaderSource = //same for light
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

const char* lightCubeFragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0);\n" // set alle 4 vector values to 1.0
"}\n";

const char* materialVertexShaderSource =
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
"	Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"	gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"}\n";

const char* materialFragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"struct Material {\n"
"	vec3 ambient;\n"
"	vec3 diffuse;\n"
"	vec3 specular;\n"
"	float shininess;\n"
"};\n"
"struct Light {\n"
"	vec3 position;\n"
"	vec3 ambient;\n"
"	vec3 diffuse;\n"
"	vec3 specular;\n"
"};\n"
"in vec3 FragPos;\n"
"in vec3 Normal;\n"
"uniform vec3 viewPos;\n"
"uniform Material material;\n"
"uniform Light light;\n"
"void main()\n"
"{\n"
"	vec3 ambient = light.ambient * material.ambient;\n" //ambient
"	vec3 norm = normalize(Normal);\n" // diffuse 
"	vec3 lightDir = normalize(light.position - FragPos);\n"
"	float diff = max(dot(norm, lightDir), 0.0);\n"
"	vec3 diffuse = light.diffuse * (diff * material.diffuse);\n"
"	vec3 viewDir = normalize(viewPos - FragPos);\n" // specular
"	vec3 reflectDir = reflect(-lightDir, norm);\n"
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
"	vec3 specular = light.specular * (spec * material.specular);\n"
"	vec3 result = ambient + diffuse + specular;\n"
"	FragColor = vec4(result, 1.0);\n"
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
	unsigned int lightCubeVertexShader = 0, materialVertexShader = 0;
	checkShaderError(
		createAndCompileShader(lightCubeVertexShaderSource, lightCubeVertexShader, GL_VERTEX_SHADER), 
		lightCubeVertexShader, 
		vertexShaderError
	);

	checkShaderError(
		createAndCompileShader(materialVertexShaderSource, materialVertexShader, GL_VERTEX_SHADER),
		materialVertexShader,
		vertexShaderError
	);

	unsigned int lightCubeFragmentShader = 0, materialFragmentShader = 0;
	checkShaderError(
		createAndCompileShader(lightCubeFragmentShaderSource, lightCubeFragmentShader, GL_FRAGMENT_SHADER),
		lightCubeFragmentShader,
		fragmentShaderError
	);

	checkShaderError(
		createAndCompileShader(materialFragmentShaderSource, materialFragmentShader, GL_FRAGMENT_SHADER),
		materialFragmentShader,
		fragmentShaderError
	);

	unsigned int lightCubeShaderProgram = 0, materialShaderProgram = 0;
	checkShaderError(
		createAndLinkShaderProgram(lightCubeVertexShader, lightCubeFragmentShader, lightCubeShaderProgram),
		lightCubeShaderProgram,
		shaderProgramError
	);

	checkShaderError(
		createAndLinkShaderProgram(materialVertexShader, materialFragmentShader, materialShaderProgram),
		materialShaderProgram,
		shaderProgramError
	);

	glDeleteShader(lightCubeVertexShader);
	glDeleteShader(lightCubeFragmentShader);
	glDeleteShader(materialVertexShader);
	glDeleteShader(materialFragmentShader);

	glUseProgram(materialShaderProgram); //Not sure what to do with this

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
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{

		processInput(window);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(materialShaderProgram);

		//Material and light stuff
		 // light properties
		glm::vec3 lightColor;
		lightColor.x = sin(glfwGetTime() * 2.0f);
		lightColor.y = sin(glfwGetTime() * 0.7f);
		lightColor.z = sin(glfwGetTime() * 1.3f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 materialAmbientColor = glm::vec3(1.0f, 0.5f, 0.31f);
		glm::vec3 materialDiffuseColor = glm::vec3(1.0f, 0.5f, 0.31f);
		glm::vec3 materialSpecularColor = glm::vec3(0.5f, 0.5f, 0.5f);
		float materialShininessColor = 32.0f;

		GLuint lightAmbientLocation = glGetUniformLocation(materialShaderProgram, "light.ambient");
		GLuint lightDiffuseLocation = glGetUniformLocation(materialShaderProgram, "light.diffuse");
		GLuint lightSpecularLocation = glGetUniformLocation(materialShaderProgram, "light.specular");
		GLuint materialAmbientLocation = glGetUniformLocation(materialShaderProgram, "material.ambient");
		GLuint materialDiffuseLocation = glGetUniformLocation(materialShaderProgram, "material.diffuse");
		GLuint materialSpecularLocation = glGetUniformLocation(materialShaderProgram, "material.specular");
		GLuint materialShininessLocation = glGetUniformLocation(materialShaderProgram, "material.shininess");


		glUniform3fv(lightAmbientLocation,1 , glm::value_ptr(ambientColor));
		glUniform3fv(lightDiffuseLocation,1 , glm::value_ptr(diffuseColor));
		glUniform3fv(lightSpecularLocation, 1, glm::value_ptr(specularColor));
		glUniform3fv(materialAmbientLocation, 1, glm::value_ptr(materialAmbientColor));
		glUniform3fv(materialDiffuseLocation, 1, glm::value_ptr(materialDiffuseColor));
		glUniform3fv(materialSpecularLocation, 1, glm::value_ptr(materialSpecularColor));
		glUniform1f(materialShininessLocation, materialShininessColor);


		//Light settings
		glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
		lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		GLuint objectColorLocation = glGetUniformLocation(materialShaderProgram, "objectColor");
		GLuint lightColorLocation = glGetUniformLocation(materialShaderProgram, "lightColor");
		GLuint lightPosLocation = glGetUniformLocation(materialShaderProgram, "lightPos");
		glUniform3fv(objectColorLocation, 1, glm::value_ptr(objectColor));
		glUniform3fv(lightColorLocation, 1, glm::value_ptr(lightColor));
		glUniform3fv(lightPosLocation, 1, glm::value_ptr(lightPos));

		// camera/view transformation
		GLuint viewMatrixLocation = glGetUniformLocation(materialShaderProgram, "view");
		GLuint projectionMatrixLocation = glGetUniformLocation(materialShaderProgram, "projection");
		GLuint modelMatrixLocation = glGetUniformLocation(materialShaderProgram, "model");


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
		glUseProgram(lightCubeShaderProgram);
		projectionMatrixLocation = glGetUniformLocation(lightCubeShaderProgram, "projection");
		viewMatrixLocation = glGetUniformLocation(lightCubeShaderProgram,"view");
		modelMatrixLocation = glGetUniformLocation(lightCubeShaderProgram, "model");
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
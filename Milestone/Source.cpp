#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Sphere.h"

#include "shader.h"

#include <iostream>

#include "cylinder.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// lighting
glm::vec3 lightPos(0.0f, 8.0f, 0.0f);
float xlight = 0.0f, ylight = 8.0f, zlight = 0.0f;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
bool orthographic = false;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// camera movement speed
float cameraSpeed;
float speedVariation = 0.0f;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Jorge Sierra Milestone", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");
	Shader lightCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");

	// configure the light's VAO
	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// PLANE
		-15.0f, -15.0f, -15.0f, 0.0f, 0.0f,
		 15.0f, -15.0f, -15.0f, 1.0f, 0.0f,
		 15.0f, -15.0f,  15.0f, 1.0f, 1.0f,
		 15.0f, -15.0f,  15.0f, 1.0f, 1.0f,
		-15.0f, -15.0f,  15.0f, 0.0f, 1.0f,
		-15.0f, -15.0f, -15.0f, 0.0f, 0.0f, 

		// ERASER
		0.0f, 0.0f,  0.0f, 0.0f, 0.0f, //6
		1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		1.0f, 0.5f,  0.0f, 1.0f, 1.0f,
		0.0f, 0.0f,  0.0f, 1.0f, 1.0f,
		0.0f, 0.5f,  0.0f, 0.0f, 1.0f,
		1.0f, 0.5f,  0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, -2.0f, 0.0f, 0.0f,
		1.0f, 0.0f, -2.0f, 1.0f, 0.0f,
		1.0f, 0.5f, -2.0f, 1.0f, 1.0f,
		0.0f, 0.0f, -2.0f, 1.0f, 1.0f,
		0.0f, 0.5f, -2.0f, 0.0f, 1.0f,
		1.0f, 0.5f, -2.0f, 0.0f, 0.0f,

		0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 0.5f,  0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, -2.0f, 1.0f, 1.0f,
		0.0f, 0.5f,  0.0f, 1.0f, 1.0f,
		0.0f, 0.5f, -2.0f, 0.0f, 1.0f,
		0.0f, 0.0f, -2.0f, 0.0f, 0.0f,

		1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		1.0f, 0.5f,  0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, -2.0f, 1.0f, 1.0f,
		1.0f, 0.5f,  0.0f, 1.0f, 1.0f,
		1.0f, 0.5f, -2.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -2.0f, 0.0f, 0.0f,

		0.0f, 0.5f,  0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -2.0f, 1.0f, 0.0f,
		1.0f, 0.5f, -2.0f, 1.0f, 1.0f,
		0.0f, 0.5f,  0.0f, 1.0f, 1.0f,
		1.0f, 0.5f,  0.0f, 0.0f, 1.0f,
		1.0f, 0.5f, -2.0f, 0.0f, 0.0f,

		0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f, 1.0f, 0.0f,
		1.0f, 0.0f, -2.0f, 1.0f, 1.0f,
		0.0f, 0.0f,  0.0f, 1.0f, 1.0f,
		1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -2.0f, 0.0f, 0.0f,

		// ERASER TOP
		0.0f, 0.0f,  0.0f, 0.0f, 0.0f, //42
		1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		1.0f, 0.1f,  0.0f, 1.0f, 1.0f,
		0.0f, 0.0f,  0.0f, 1.0f, 1.0f,
		0.0f, 0.1f,  0.0f, 0.0f, 1.0f,
		1.0f, 0.1f,  0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, -2.0f, 0.0f, 0.0f,
		1.0f, 0.0f, -2.0f, 1.0f, 0.0f,
		1.0f, 0.1f, -2.0f, 1.0f, 1.0f,
		0.0f, 0.0f, -2.0f, 1.0f, 1.0f,
		0.0f, 0.1f, -2.0f, 0.0f, 1.0f,
		1.0f, 0.1f, -2.0f, 0.0f, 0.0f,

		0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 0.1f,  0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, -2.0f, 1.0f, 1.0f,
		0.0f, 0.1f,  0.0f, 1.0f, 1.0f,
		0.0f, 0.1f, -2.0f, 0.0f, 1.0f,
		0.0f, 0.0f, -2.0f, 0.0f, 0.0f,

		1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		1.0f, 0.1f,  0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, -2.0f, 1.0f, 1.0f,
		1.0f, 0.1f,  0.0f, 1.0f, 1.0f,
		1.0f, 0.1f, -2.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -2.0f, 0.0f, 0.0f,

		0.0f, 0.1f,  0.0f, 0.0f, 0.0f,
		0.0f, 0.1f, -2.0f, 1.0f, 0.0f,
		1.0f, 0.1f, -2.0f, 1.0f, 1.0f,
		0.0f, 0.1f,  0.0f, 1.0f, 1.0f,
		1.0f, 0.1f,  0.0f, 0.0f, 1.0f,
		1.0f, 0.1f, -2.0f, 0.0f, 0.0f,

		0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f, 1.0f, 0.0f,
		1.0f, 0.0f, -2.0f, 1.0f, 1.0f,
		0.0f, 0.0f,  0.0f, 1.0f, 1.0f,
		1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -2.0f, 0.0f, 0.0f,

		// BOTTLE BODY
		0.0f, 0.0f,  0.0f, 0.0f, 1.0f, //78
		1.5f, 0.0f,  0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, -1.5f, 0.0f, 0.0f,
		1.5f, 0.0f,  0.0f, 1.0f, 1.0f,
		1.5f, 0.0f, -1.5f, 1.0f, 0.0f,
		0.0f, 0.0f, -1.5f, 0.0f, 0.0f,
			  
		0.0f, 3.0f,  0.0f, 0.0f, 1.0f,
		1.5f, 3.0f,  0.0f, 1.0f, 1.0f,
		0.0f, 3.0f, -1.5f, 0.0f, 0.0f,
		1.5f, 3.0f,  0.0f, 1.0f, 1.0f,
		1.5f, 3.0f, -1.5f, 1.0f, 0.0f,
		0.0f, 3.0f, -1.5f, 0.0f, 0.0f,

		0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 3.0f,  0.0f, 0.0f, 1.0f,
		0.0f, 3.0f, -1.5f, 1.0f, 1.0f,
		0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.5f, 1.0f, 0.0f,
		0.0f, 3.0f, -1.5f, 1.0f, 1.0f,

		1.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		1.5f, 3.0f,  0.0f, 1.0f, 1.0f,
		1.5f, 3.0f, -1.5f, 0.0f, 1.0f,
		1.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		1.5f, 0.0f, -1.5f, 0.0f, 0.0f,
		1.5f, 3.0f, -1.5f, 0.0f, 1.0f,

		0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		0.0f, 3.0f,  0.0f, 1.0f, 1.0f,
		1.5f, 0.0f,  0.0f, 0.0f, 0.0f,
		1.5f, 0.0f,  0.0f, 0.0f, 0.0f,
		1.5f, 3.0f,  0.0f, 0.0f, 1.0f,
		0.0f, 3.0f,  0.0f, 1.0f, 1.0f,

		0.0f, 0.0f, -1.5f, 0.0f, 0.0f,
		0.0f, 3.0f, -1.5f, 0.0f, 1.0f,
		1.5f, 0.0f, -1.5f, 1.0f, 0.0f,
		1.5f, 0.0f, -1.5f, 1.0f, 0.0f,
		1.5f, 3.0f, -1.5f, 1.0f, 1.0f,
		0.0f, 3.0f, -1.5f, 0.0f, 1.0f,


		// BOTTLE TOP
		0.0f, 0.0f,  0.0f, 0.0f, 0.0f, //114
		1.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		1.5f, 0.0f, -1.5f, 1.0f, 1.0f,

		0.0f, 0.0f,  0.0f, 1.0f, 1.0f,
		1.5f, 0.0f, -1.5f, 0.0f, 1.0f,
		0.0f, 0.0f, -1.5f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.75f, 1.0f, -0.75f, 0.0f, 1.0f,

		1.5f, 0.0f, -1.5f, 0.0f, 0.0f,
		1.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.75f, 1.0f, -0.75f, 0.0f, 1.0f,

		1.5f, 0.0f, -1.5f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.5f, 1.0f, 0.0f,
		0.75f, 1.0f, -0.75f, 0.0f, 1.0f,

		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.5f, 1.0f, 0.0f,
		0.75f, 1.0f, -0.75f, 0.0f, 1.0f

	};

	unsigned int VBO, VAO;
	unsigned int VBO1, VAO1;
	unsigned int VBO2, VAO2;
	unsigned int VBO3, VAO3;
	unsigned int VBO4, VAO4;
	unsigned int VBO5, VAO5;
	unsigned int VBO6, VAO6;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glGenVertexArrays(1, &VAO1);
	glGenBuffers(1, &VBO1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);

	glGenVertexArrays(1, &VAO3);
	glGenBuffers(1, &VBO3);
	glBindVertexArray(VAO3);
	glBindBuffer(GL_ARRAY_BUFFER, VBO3);

	glGenVertexArrays(1, &VAO4);
	glGenBuffers(1, &VBO4);
	glBindVertexArray(VAO4);
	glBindBuffer(GL_ARRAY_BUFFER, VBO4);

	glGenVertexArrays(1, &VAO5);
	glGenBuffers(1, &VBO5);
	glBindVertexArray(VAO5);
	glBindBuffer(GL_ARRAY_BUFFER, VBO5);

	glGenVertexArrays(1, &VAO6);
	glGenBuffers(1, &VBO6);
	glBindVertexArray(VAO6);
	glBindBuffer(GL_ARRAY_BUFFER, VBO6);


	// load and create a texture 
	// -------------------------
	unsigned int texture0, texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8;
	// texture 0
	// ---------
	glGenTextures(1, &texture0);
	glBindTexture(GL_TEXTURE_2D, texture0);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("images/table1.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	data = stbi_load("images/cookie1.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	// texture 2
	// ---------
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	unsigned char* data1 = stbi_load("images/cookie2.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data1);

	// texture 3
	// ---------
	glGenTextures(1, &texture3);
	glBindTexture(GL_TEXTURE_2D, texture3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data2 = stbi_load("images/black.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data2);

	// texture 4
	// ---------
	glGenTextures(1, &texture4);
	glBindTexture(GL_TEXTURE_2D, texture4);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data3 = stbi_load("images/eraser2.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data3);

	// texture 5
	// ---------
	glGenTextures(1, &texture5);
	glBindTexture(GL_TEXTURE_2D, texture5);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data4 = stbi_load("images/bottle1.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data4);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data4);

	// texture 6
	// ---------
	glGenTextures(1, &texture6);
	glBindTexture(GL_TEXTURE_2D, texture6);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data5 = stbi_load("images/bottle2.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data5);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data5);

	// texture 7
	// ---------
	glGenTextures(1, &texture7);
	glBindTexture(GL_TEXTURE_2D, texture7);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data6 = stbi_load("images/marker2.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data6);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data6);

	// texture 8
	// ---------
	glGenTextures(1, &texture8);
	glBindTexture(GL_TEXTURE_2D, texture8);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data7 = stbi_load("images/marker3.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data7);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data7);


	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
	// -------------------------------------------------------------------------------------------
	ourShader.use();
	ourShader.setInt("texture0", 0);
	ourShader.setInt("texture1", 1);
	ourShader.setInt("texture2", 2);
	ourShader.setInt("texture3", 3);
	ourShader.setInt("texture4", 4);
	ourShader.setInt("texture5", 5);
	ourShader.setInt("texture6", 6);
	ourShader.setInt("texture7", 7);
	ourShader.setInt("texture8", 8);

	glm::mat4 model;
	float angle;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		lightPos[0] = xlight;
		lightPos[1] = ylight;
		lightPos[2] = zlight;

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);

		// activate shader
		ourShader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection;
		if (orthographic) {
			float scale = 100;
			projection = glm::ortho(-((float)SCR_WIDTH / scale), ((float)SCR_WIDTH / scale), -((float)SCR_HEIGHT / scale), ((float)SCR_HEIGHT / scale), -10.0f, 100.0f);
		}
		else {
			projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		}
		
		ourShader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", view);

		// OBJECTS

		// PLANE
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture0);
		glBindVertexArray(VAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 14.5f, 0.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// COOKIE CAN CAP
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glBindVertexArray(VAO1);
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 0.6f, 0.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C(2.2, 20, 0.3, true, true, true);
		C.render();

		// COOKIE CAN BODY

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glBindVertexArray(VAO2);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C1(2, 20, 1, true, true, true);
		C1.render();

		// ERASER
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glBindVertexArray(VAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(3.0f, -0.5f, 7.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 6, 36);

		// ERASER TOP
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture4);
		glBindVertexArray(VAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(3.0f, 0.5f, 7.0f));
		model = glm::scale(model, glm::vec3(2.0f));
		model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 42, 36);

		// BOTTLE BODY
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture5);
		glBindVertexArray(VAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-5.0f, -0.5f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 78, 36);

		// BOTTLE TOP
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture6);
		glBindVertexArray(VAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-5.0f, 4.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.5f));
		model = glm::rotate(model, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ourShader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 114, 18);

		// BOTTLE CAP
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture6);
		glBindVertexArray(VAO3);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-4.2f, 5.3f, -1.37f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C2(0.5, 10, 0.5, true, true, true);
		C2.render();

		// MARKER BOTTOM
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture8);
		glBindVertexArray(VAO4);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(1.0f, -0.3f, 4.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.5f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C3(0.2, 10, 0.1, true, true, true);
		C3.render();

		// MARKER BODY
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture7);
		glBindVertexArray(VAO5);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(0.44f, -0.28f, 5.15f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.5f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C4(0.25, 10, 2.5, true, true, true);
		C4.render();

		// MARKER CAP
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture8);
		glBindVertexArray(VAO6);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-0.35f, -0.28f, 6.7f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.5f));
		ourShader.setMat4("model", model);

		static_meshes_3D::Cylinder C5(0.25, 10, 1.0, true, true, true);
		C5.render();

		// LIGHT SOURCE
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.5f));
		lightCubeShader.setMat4("model", model);

		static_meshes_3D::Cylinder C6(1.6, 30, 1.5, true, true, true);
		C6.render();

		glBindVertexArray(lightCubeVAO);

		


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	cameraSpeed = 2.5 * deltaTime + speedVariation;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		orthographic = !orthographic;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	speedVariation += 0.01 * (float)yoffset;
	if (speedVariation < 0.0)
		speedVariation = 0.0;
}
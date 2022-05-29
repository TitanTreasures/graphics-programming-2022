#define _USE_MATH_DEFINES
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <shader.h>
#include <camera.h>

// structure to hold the info necessary to render an object
struct SceneObject {
	unsigned int VAO;           // vertex array object handle
	unsigned int vertexCount;   // number of vertices in the object
	unsigned int indecesCount;  // number of vertices in the object
	glm::mat4 position;			// position in world space
	unsigned int texture1, texture2;		// textures
	unsigned int heightmap;
};

// structure to hold lighting info
// -------------------------------
struct Config {

	// ambient light
	glm::vec3 ambientLightColor = { 1.0f, 1.0f, 1.0f };
	float ambientLightIntensity = 1.0f;

	// light 1
	glm::vec3 light1Position = { -4.0f, -4.0f, -4.0f };
	glm::vec3 light1Color = { 1.0f, 1.0f, 1.0f };
	float light1Intensity = 1.0f;

	// light 2
	glm::vec3 light2Position = { 4.0f, 4.0f, 4.0f };
	glm::vec3 light2Color = { 1.0f, 1.0f, 1.0f };
	float light2Intensity = 10.0f;

	// material
	glm::vec3 reflectionColor = { 1.0f, 1.0f, 1.0f };
	float ambientReflectance = 0.5f;
	float diffuseReflectance = 0.5f;
	float specularReflectance = 0.7f;
	float specularExponent = 20.0f;

} config;

// Vertex structure
struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
};

// declaration of the function you will implement in exercise 2.1
SceneObject instantiateSphere();
// mouse, keyboard and screen reshape glfw callbacks
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_input_callback(GLFWwindow* window, int button, int other, int action, int mods);
void cursor_input_callback(GLFWwindow* window, double posX, double posY);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void setLightUniforms();

void FPSUpdate();

// global variables we will use to store our objects, shaders, and active shader
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const char* windowName = "Volatile Flame Project ";
GLFWwindow* window;
bool showWireframe = false;
Shader* phong_shading;
Shader* shader;
std::vector<SceneObject> sceneObjects;
double prevTime = 0.0;
double crntTime = 0.0;
unsigned int counter = 0;
Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
float deltaTime;
bool isPaused = false; // stop camera movement when GUI is open


int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwSwapInterval(60);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, windowName, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// setup frame buffer size callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// setup input callbacks
	glfwSetCursorPosCallback(window, cursor_input_callback);
	glfwSetKeyCallback(window, key_input_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// build and compile the shader program
	phong_shading = new Shader("shaders/phong_shading.vert", "shaders/phong_shading.frag");
	shader = phong_shading;
	//&Shader("shaders/shader.vert", "shaders/color.frag");

	// set up the z-buffer
	glDepthRange(-1, 1); // make the NDC a right handed coordinate system, with the camera pointing towards -z
	glEnable(GL_DEPTH_TEST); // turn on z-buffer depth test
	glDepthFunc(GL_LESS); // draws fragments that are closer to the screen in NDC

	// Create a sphere
	sceneObjects.push_back(instantiateSphere());

	//Set alpha debug
	float alpha = 1;


	// render loop
	while (!glfwWindowShouldClose(window)) {
		FPSUpdate();

		processInput(window);

		// background color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// notice that now we are clearing two buffers, the color and the z-buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// select shader to use
		shader->use();

		//setup light setting uniforms
		setLightUniforms();

		// camera position
		shader->setVec3("camPosition", camera.Position);

		// material uniforms
		shader->setVec3("reflectionColor", config.reflectionColor);
		shader->setFloat("ambientReflectance", config.ambientReflectance);
		shader->setFloat("diffuseReflectance", config.diffuseReflectance);
		shader->setFloat("specularReflectance", config.specularReflectance);
		shader->setFloat("specularExponent", config.specularExponent);

		// camera parameters
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 viewProjection = projection * view;

		// set viewProjection matrix uniform
		shader->setMat4("viewProjection", viewProjection);
		alpha += deltaTime;
		//float opacity = (sin(alpha) / 2) + 0.5f;
		//shader->setFloat("mixValue", opacity);
		shader->setFloat("delta", deltaTime);
		if (alpha>10) {
			alpha = 1;
		}

		for (int i = 0; i < sceneObjects.size(); i++) {

			SceneObject SO = sceneObjects[i];

			glActiveTexture(GL_TEXTURE0 + sceneObjects[i].texture1);
			glBindTexture(GL_TEXTURE_2D, sceneObjects[i].texture1);
			glActiveTexture(GL_TEXTURE0 + sceneObjects[i].texture2);
			glBindTexture(GL_TEXTURE_2D, sceneObjects[i].texture2);
			glActiveTexture(GL_TEXTURE0 + sceneObjects[i].heightmap);
			glBindTexture(GL_TEXTURE_2D, sceneObjects[i].heightmap);

			// bind vertex array object
			glBindVertexArray(sceneObjects[i].VAO);

			// Set the position of the object for this frame
			//sceneObjects[i].position = glm::rotate(sceneObjects[i].position, glm::radians(deltaTime*10), glm::vec3(0, 0, 1));
			shader->setMat4("modelToWorldSpace", sceneObjects[i].position);

			// draw geometry
			glDrawElements(GL_TRIANGLES, sceneObjects[i].indecesCount, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

void setLightUniforms()
{
	// TODO exercise 5 - set the missing uniform variables here
	// light uniforms
	shader->setVec3("ambientLightColor", config.ambientLightColor * config.ambientLightIntensity);
	shader->setVec3("light1Position", config.light1Position);
	shader->setVec3("light1Color", config.light1Color * config.light1Intensity);
	shader->setVec3("light2Position", config.light2Position);
	shader->setVec3("light2Color", config.light2Color * config.light2Intensity);
}

//inspiration: http://www.songho.ca/opengl/gl_sphere.html
// creates a cone triangle mesh, uploads it to openGL and returns the VAO associated to the mesh
SceneObject instantiateSphere() {

	// Create an instance of a SceneObject
	SceneObject sceneObject;

	// Generate texture. Code from https://learnopengl.com/Getting-started/Textures
	unsigned int lavaTexture;
	glGenTextures(1, &lavaTexture);
	glBindTexture(GL_TEXTURE_2D, lavaTexture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width1, height1, nrChannels1;
	unsigned char* data1 = stbi_load("C:/Users/TitanTreasures/Documents/Git/graphics-programming-2022/VolatileFlameProject/Textures/lava.jpg", &width1, &height1, &nrChannels1, 0);
	if (data1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data1);

	sceneObject.texture1 = lavaTexture;

	unsigned int rockTexture;
	glGenTextures(1, &rockTexture);
	glBindTexture(GL_TEXTURE_2D, rockTexture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width2, height2, nrChannels2;
	unsigned char* data2 = stbi_load("C:/Users/TitanTreasures/Documents/Git/graphics-programming-2022/VolatileFlameProject/Textures/rock.jpg", &width2, &height2, &nrChannels2, 0);
	if (data2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data2);

	sceneObject.texture2 = rockTexture;


	// Load the heightmap texture
	unsigned int heightMap;
	glGenTextures(1, &heightMap);
	glBindTexture(GL_TEXTURE_2D, heightMap);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width3, height3, nrChannels3;
	unsigned char* data3 = stbi_load("C:/Users/TitanTreasures/Documents/Git/graphics-programming-2022/VolatileFlameProject/Textures/texture3.jpg", &width3, &height3, &nrChannels3, 0);
	if (data3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, data3);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load heightmap texture" << std::endl;
	}
	//std::cout << data3 << "\n";
	stbi_image_free(data3);
	sceneObject.heightmap = heightMap;

	shader->use();
	glUniform1i(glGetUniformLocation(shader->ID, "texture1"), sceneObject.texture1);
	glUniform1i(glGetUniformLocation(shader->ID, "texture2"), sceneObject.texture2);
	glUniform1i(glGetUniformLocation(shader->ID, "heightmap"), sceneObject.heightmap);

	// Object position offset
	glm::mat4 position = glm::mat4(1.0f);
	sceneObject.position = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	// rotate 90 degrees on the x-axis
	sceneObject.position = glm::rotate(sceneObject.position, glm::radians(90.0f), glm::vec3(1, 0, 0));

	// Some code from http://www.songho.ca/opengl/gl_sphere.html
	// <->
	float multiplier = 10.0f;
	float sectorCount = 10.0f*multiplier;
	// ^-v
	float stackCount = 5.0f*multiplier;
	// O-|
	float radius = 1.0f;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * M_PI / sectorCount;
	float stackStep = M_PI / stackCount;
	float sectorAngle, stackAngle;

	std::vector<Vertex> vertices;
	std::vector<float> heights;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			glm::vec3 pos(x, y, z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			glm::vec3 norm(nx, ny, nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			glm::vec2 tex(s, t);

			//heights.push_back();

			Vertex vert;
			vert.Position = pos;
			vert.Normal = norm;
			vert.TexCoords = tex;
			vertices.push_back(vert);
		}
	}

	std::vector<unsigned int> indices;
	std::vector<int> lineIndices;
	unsigned int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				indices.push_back(k1);
				indices.push_back(k2);
				indices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				indices.push_back(k1 + 1);
				indices.push_back(k2);
				indices.push_back(k2 + 1);
			}

			// store indices for lines
			// vertical lines for all stacks, k1 => k2
			lineIndices.push_back(k1);
			lineIndices.push_back(k2);
			if (i != 0)  // horizontal lines except 1st stack, k1 => k+1
			{
				lineIndices.push_back(k1);
				lineIndices.push_back(k1 + 1);
			}
		}
	}

	// Store the number of vertices in the mesh in the scene object.
	sceneObject.vertexCount = vertices.size() / 3;
	sceneObject.indecesCount = indices.size();

	// Declare and generate a VAO and VBO (and an EBO if you decide the work with indices).
	unsigned int EBO, VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind and set the VAO and VBO (and optionally a EBO) in the correct order.
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Set the position attribute pointers in the shader.

	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// Store the VAO handle in the scene object.
	sceneObject.VAO = VAO;

	// Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::cout << "Created Sphere with " << vertices.size() << " vertices, " << indices.size() << " indices" << std::endl;
	// 'return' the scene object for the cone instance you just created.
	return sceneObject;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (isPaused)
		return;

	// movement commands
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}


void cursor_input_callback(GLFWwindow* window, double posX, double posY) {

	// camera rotation
	static bool firstMouse = true;
	if (firstMouse)
	{
		lastX = (float)posX;
		lastY = (float)posY;
		firstMouse = false;
	}

	float xoffset = (float)posX - lastX;
	float yoffset = lastY - (float)posY; // reversed since y-coordinates go from bottom to top

	lastX = (float)posX;
	lastY = (float)posY;

	if (isPaused)
		return;

	// we use the handy camera class from LearnOpenGL to handle our camera
	camera.ProcessMouseMovement(xoffset, yoffset);
}


void key_input_callback(GLFWwindow* window, int button, int other, int action, int mods) {
	// controls pause mode
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		isPaused = !isPaused;
		glfwSetInputMode(window, GLFW_CURSOR, isPaused ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
	if (button == GLFW_KEY_E && action == GLFW_PRESS) {
		showWireframe = !showWireframe;
		if (showWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll((float)yoffset);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void FPSUpdate()
{
	static float lastFrame = 0.0f;
	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Creates new title
	int FPS = (1.0 / deltaTime);
	std::string newTitle = windowName + std::to_string(FPS) + " FPS / " + std::to_string(deltaTime) + " ms";
	glfwSetWindowTitle(window, newTitle.c_str());
}

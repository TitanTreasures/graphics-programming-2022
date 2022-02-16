#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <iostream>
#include <vector>
#include <math.h>

// structure to hold the info necessary to render an object
struct SceneObject {
	unsigned int VAO;           // vertex array object handle
	unsigned int vertexCount;   // number of vertices in the object
	unsigned int indecesCount;   // number of vertices in the object
	float r, g, b;              // for object color
	float x, y;                 // for position offset
};

// declaration of the function you will implement in exercise 2.1
SceneObject instantiateCone(float r, float g, float b, float offsetX, float offsetY);
// mouse, keyboard and screen reshape glfw callbacks
void cursor_position_callback(GLFWwindow* window, double xPos, double yPos);
void button_input_callback(GLFWwindow* window, int button, int action, int mods);
void key_input_callback(GLFWwindow* window, int button, int other, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void FPSUpdate();

// variables
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;
glm::vec2 cursorPos;
const char* windowName = "Exercise 2 - Voronoi Diagram ";
GLFWwindow* window;
bool showWireframe = false;

// FPS variables
double prevTime = 0.0;
double crntTime = 0.0;
double timeDiff;
unsigned int counter = 0;

// global variables we will use to store our objects, shaders, and active shader
std::vector<SceneObject> sceneObjects;
std::vector<Shader> shaderPrograms;
Shader* activeShader;

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
	glfwSetMouseButtonCallback(window, button_input_callback); // NEW!
	glfwSetKeyCallback(window, key_input_callback); // NEW!
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// NEW!
	// build and compile the shader programs
	shaderPrograms.push_back(Shader("shaders/shader.vert", "shaders/color.frag"));
	shaderPrograms.push_back(Shader("shaders/shader.vert", "shaders/distance.frag"));
	shaderPrograms.push_back(Shader("shaders/shader.vert", "shaders/distance_color.frag"));
	activeShader = &shaderPrograms[0];

	// NEW!
	// set up the z-buffer
	glDepthRange(1, -1); // make the NDC a right handed coordinate system, with the camera pointing towards -z
	glEnable(GL_DEPTH_TEST); // turn on z-buffer depth test
	glDepthFunc(GL_LESS); // draws fragments that are closer to the screen in NDC

	// TODO exercise 2.6
	// enable blending
	// choose the right blending factors to produce additive blending
	// glBlendFunc(?, ?);

	// render loop
	while (!glfwWindowShouldClose(window)) {
		FPSUpdate();

		// background color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// notice that now we are clearing two buffers, the color and the z-buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render the cones
		glUseProgram(activeShader->ID);

		for (int i = 0; i < sceneObjects.size(); i++) {
			// bind vertex array object
			SceneObject SO = sceneObjects[i];
			glBindVertexArray(sceneObjects[i].VAO);
			if (activeShader == &shaderPrograms[0] || activeShader == &shaderPrograms[2]) {
				GLint id = glGetUniformLocation(activeShader->ID, "uColor");
				if (id == -1) {
					std::cout << "id not found here" << std::endl;
				}
				glUniform3f(id, SO.r, SO.g, SO.b);
			}
			GLint id = glGetUniformLocation(activeShader->ID, "uPosition");
			if (id == -1) {
				std::cout << "id not found here" << std::endl;
			}
			glUniform2f(id, SO.x, SO.y);

			// draw geometry
			glDrawElements(GL_TRIANGLES, sceneObjects[0].indecesCount, GL_UNSIGNED_INT, 0);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// creates a cone triangle mesh, uploads it to openGL and returns the VAO associated to the mesh
SceneObject instantiateCone(float r, float g, float b, float offsetX, float offsetY) {
	// TODO exercise 2.1
	// (exercises 1.7 and 1.8 can help you with implementing this function)

	// Create an instance of a SceneObject
	SceneObject sceneObject;

	// you will need to store offsetX, offsetY, r, g and b in the object.
	sceneObject.r = r;
	sceneObject.g = g;
	sceneObject.b = b;

	sceneObject.x = offsetX;
	sceneObject.y = offsetY;

	// Build the geometry into an std::vector<float> or float array.
	int triangles = 30;
	float angle = 360.0f / triangles;
	std::vector<float> vertices;

	vertices.push_back(0.0f);
	vertices.push_back(0.0f);
	vertices.push_back(0.0f);

	for (int i = 0; i < triangles; i++) {
		vertices.push_back(((float)cos(glm::radians(i * angle))) / 2);
		vertices.push_back(((float)sin(glm::radians(i * angle))) / 2);
		vertices.push_back(0.0f);
	}

	vertices.push_back(0.0f);
	vertices.push_back(0.0f);
	vertices.push_back(1.0f);

	std::vector<unsigned int> indices;
	for (int i = 0; i < triangles; i++) {
		indices.push_back(0);
		float index = i + 1;
		indices.push_back(index);
		index++;
		if (index > triangles) {
			index = 1;
		}
		indices.push_back(index);
	}
	int topPoint = (vertices.size() / 3) - 1;
	for (int i = 0; i < triangles; i++) {
		indices.push_back(topPoint);
		float index = i + 1;
		indices.push_back(index);
		index++;
		if (index > triangles) {
			index = 1;
		}
		indices.push_back(index);
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
	glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(GLfloat)), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (indices.size() * sizeof(GLfloat)), &indices[0], GL_STATIC_DRAW);

	// Set the position attribute pointers in the shader.
	int posSize = 3;
	int colorSize = 3;
	int posAttributeLocation = glGetAttribLocation(activeShader->ID, "pos");

	glEnableVertexAttribArray(posAttributeLocation);

	glVertexAttribPointer(posAttributeLocation, posSize, GL_FLOAT, GL_FALSE, posSize * sizeof(GLfloat), 0);

	// Store the VAO handle in the scene object.
	sceneObject.VAO = VAO;

	// Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::cout << "Created cones: " << sceneObjects.size() << " with offset: " << offsetX << " , " << offsetY << std::endl;
	std::cout << "Color: r:" << sceneObject.r << " g: " << sceneObject.g << " b: " << sceneObject.b << std::endl;
	// 'return' the scene object for the cone instance you just created.
	return sceneObject;
}

void cursor_position_callback(GLFWwindow* window, double xPos, double yPos) {
	cursorPos.x = xPos;
	cursorPos.y = yPos;
}

// glfw: called whenever a mouse button is pressed
void button_input_callback(GLFWwindow* window, int button, int action, int mods) {
	// TODO exercise 2.2
	// (exercises 1.9 and 2.2 can help you with implementing this function)
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		// convert mouse position
		float xPosion = (cursorPos.x * 2) / ((float)(SCR_WIDTH - 1));
		float yPosion = (cursorPos.y * 2) / ((float)(SCR_HEIGHT - 1));
		yPosion = 2.0f - yPosion;
		xPosion--;
		yPosion--;
		//std::cout << "x: " << xPosion << " y: " << yPosion << std::endl;
		sceneObjects.push_back(instantiateCone((rand() % 101) / 100.0f, (rand() % 101) / 100.0f, (rand() % 101) / 100.0f, xPosion, yPosion));
	}
}

// glfw: called whenever a keyboard key is pressed
void key_input_callback(GLFWwindow* window, int button, int other, int action, int mods) {
	if (button == GLFW_KEY_1 && action == GLFW_PRESS) {
		activeShader = &shaderPrograms[0];
		std::cout << "Active shader: 0 (color.frag), ID: " << activeShader->ID << std::endl;
	}

	if (button == GLFW_KEY_2 && action == GLFW_PRESS) {
		activeShader = &shaderPrograms[1];
		std::cout << "Active shader: 1 (distance.frag), ID: " << activeShader->ID << std::endl;
	}

	if (button == GLFW_KEY_3 && action == GLFW_PRESS) {
		activeShader = &shaderPrograms[2];
		std::cout << "Active shader: 2 (distance_color.frag), ID: " << activeShader->ID << std::endl;
	}
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
	// Updates counter and times
	crntTime = glfwGetTime();
	timeDiff = crntTime - prevTime;
	counter++;

	if (timeDiff >= 1.0 / 30.0)
	{
		// Creates new title
		int FPS = (1.0 / timeDiff) * counter;
		int ms = (timeDiff / counter) * 1000;
		std::string newTitle = windowName + std::to_string(FPS) + " FPS / " + std::to_string(ms) + " ms";
		glfwSetWindowTitle(window, newTitle.c_str());

		// Resets times and counter
		prevTime = crntTime;
		counter = 0;
	}
}
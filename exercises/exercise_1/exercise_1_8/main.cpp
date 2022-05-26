#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <glm/trigonometric.hpp>
#include <string>
#include <iomanip>

// function declarations
// ---------------------
void createArrayBuffer(const std::vector<float> &array, unsigned int &VBO);
void setupShape(unsigned int shaderProgram, unsigned int &VAO, unsigned int &vertexCount);
void draw(unsigned int shaderProgram, unsigned int VAO, unsigned int vertexCount);
void FPSUpdate();

// glfw functions
// --------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// variables
// --------
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const char* windowName = "LearnOpenGL ";
GLFWwindow* window;
bool showWireframe = false;

// FPS variables
double prevTime = 0.0;
double crntTime = 0.0;
double timeDiff;
unsigned int counter = 0;

// shader programs
// ---------------
const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec3 aColor;\n"
                                 "out vec3 vtxColor; // output a color to the fragment shader\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "   vtxColor = aColor;\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "in  vec3 vtxColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(vtxColor, 1.0);\n"
                                   "}\n\0";

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, windowName, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, key_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // build and compile our shader program
    // ------------------------------------

    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    int triangles = 300;
    float split = 1.0f / ((float)triangles / 3.0f);
    std::cout << split << std::endl;
    float angle = 360.0f / triangles;
    std::vector<float> vertices;
    std::vector<float> rgb{ 0.0f,0.0f,0.0f };

        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

    int j = 0;
    for (int i = 0; i < triangles; i++) {
        if (j >= 3) {
            j = 0;
        }

        rgb[j] += split;
        if (rgb[j] > 1.0f) {
            rgb[j] = 0.0f;
            j++;
        }

        vertices.push_back(((float)cos(glm::radians(i * angle))) / 2);
        vertices.push_back(((float)sin(glm::radians(i * angle))) / 2);
        vertices.push_back(0.0f);
        vertices.push_back(rgb[0]);
        vertices.push_back(rgb[1]);
        vertices.push_back(rgb[2]);
    }

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

    //std::vector<float> indicesData;
    //for (int i = 0; i < vertices.size(); i++) {

    //    auto it = find(indicesData.begin(), indicesData.end(), vertices[i]);

    //    if (it != indicesData.end())
    //    {
    //        int index = it - indicesData.begin();
    //        indices.push_back(index);
    //    }
    //    else {
    //        indicesData.push_back(vertices[i]);
    //        int index = indicesData.size() - 1;
    //        indices.push_back(index);
    //    }
    //}

    unsigned int EBO, VAO, VBO, vertexCount;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(GLfloat)), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (indices.size() * sizeof(GLfloat)), &indices[0], GL_STATIC_DRAW);

    vertexCount = vertices.size()/6;

    int posSize = 3;
    int colorSize = 3;
    int posAttributeLocation = glGetAttribLocation(shaderProgram, "aPos");
    int colorAttributeLocation = glGetAttribLocation(shaderProgram, "aColor");

    glEnableVertexAttribArray(posAttributeLocation);
    glEnableVertexAttribArray(colorAttributeLocation);

    glVertexAttribPointer(posAttributeLocation, posSize, GL_FLOAT, GL_FALSE, (sizeof(GLfloat) * posSize) + (sizeof(GLfloat) * colorSize), 0);
    glVertexAttribPointer(colorAttributeLocation, colorSize, GL_FLOAT, GL_FALSE, (sizeof(GLfloat) * posSize) + (sizeof(GLfloat) * colorSize), (void*)(sizeof(GLfloat) * posSize));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {

        FPSUpdate();

        // render
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // background
        glClear(GL_COLOR_BUFFER_BIT); // clear the framebuffer

        // rotate all the vertices data
        float rotationSpeed = timeDiff;
        for (int i = 6; i < vertices.size(); i += 6) {
            float newX = 0 + (vertices[i] - 0) * cos(glm::radians(rotationSpeed)) - (vertices[i + 1] - 0) * sin(glm::radians(rotationSpeed));
            float newY = 0 + (vertices[i] - 0) * sin(glm::radians(rotationSpeed)) + (vertices[i + 1] - 0) * cos(glm::radians(rotationSpeed));
            vertices[i] = newX;
            vertices[i + 1] = newY;
        }

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(GLfloat)), &vertices[0], GL_STATIC_DRAW);

        // set active shader program
        glUseProgram(shaderProgram);
        // bind vertex array object
        glBindVertexArray(VAO);
        // draw geometry
        //glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window); // we normally use 2 frame buffers, a back (to draw on) and a front (to show on the screen)
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
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

void FPSUpdate()
{
    // Updates counter and times
    crntTime = glfwGetTime();
    timeDiff = crntTime - prevTime;
    counter++;

    if (timeDiff >= 1.0 / 30.0)
    {
        // Creates new title
        std::setprecision(2);
        int FPS = (1.0 / timeDiff) * counter;
        int ms = (timeDiff / counter) * 1000;
        std::string newTitle = windowName + std::to_string(FPS) + " FPS / " + std::to_string(ms) + " ms";
        glfwSetWindowTitle(window, newTitle.c_str());

        // Resets times and counter
        prevTime = crntTime;
        counter = 0;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


#include <iostream>
// include glad first, contains OpenGL headers behind the scenes
#include <glad/glad.h>
// Open GL FrameWork
#include <GLFW/glfw3.h>

// Define functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// source code for the vertex shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// source code for the fragment shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

// wireframe bool
bool showWireframe = false;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    // The options that we want:
    //https://www.glfw.org/docs/latest/window.html#window_hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lecture 1 Assignment", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    //Vertex input for the Vertex shader (square)
    float vertices[] = {
        0.5f, 0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f, 0.5f, 0.0f   // top left 
};

    unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
    };

    // VERTEX BUFFER
    // Create a vertex buffer object with ID '1' to store the data on the graphics card (sending data to the GPU from the CPU is slow)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    //  OpenGL allows us to bind to several buffers at once as long as they have a different buffer type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // copies the previously defined vertex data into the buffer's memory:
    // GL_STREAM_DRAW: the data is set only onceand used by the GPU at most a few times.
    // GL_STATIC_DRAW : the data is set only onceand used many times.
    // GL_DYNAMIC_DRAW : the data is changed a lotand used many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // VERTEX SHADER
    // create a shader object
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // attach the shader source code to the shader object and compile the shader:
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check if compilation was successful after the call to glCompileShader
    int  vsuccess;
    char vinfoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vsuccess);
    if (!vsuccess)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, vinfoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vinfoLog << std::endl;
    }

    // FRAGMENT SHADER
    // create a fragment shader object
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // attach the fragment shader source code to the shader object and compile the shader:
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check if compilation was successful after the call to glCompileShader
    int  fsuccess;
    char finfoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fsuccess);
    if (!fsuccess)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, finfoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << finfoLog << std::endl;
    }

    // SHADER PROGRAM
    // create shader program:
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // attach the previously compiled shaders to the program objectand then link them with glLinkProgram :
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check if linking a shader program failed and retrieve the corresponding log
    int  ssuccess;
    char sinfoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ssuccess);
    if (!ssuccess) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, sinfoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << sinfoLog << std::endl;
    }

    // The result is a program object that we can activate by calling glUseProgram with the newly created program object as its argument:
    // Every shader and rendering call after glUseProgram will now use this program object (and thus the shaders).
    glUseProgram(shaderProgram);

    // delete the shader objects once we've linked them into the program object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // LINKING VERTEX ATTRIBUTES
    // tell OpenGL how it should interpret the vertex data (per vertex attribute) using glVertexAttribPointer:
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*
                ^
                |
    The first parameter specifies which vertex attribute we want to configure.
    Remember that we specified the location of the position vertex attribute in the vertex shader with layout(location = 0).
    This sets the location of the vertex attribute to 0 and since we want to pass data to this vertex attribute, we pass in 0.
        
    The next argument specifies the size of the vertex attribute.The vertex attribute is a vec3 so it is composed of 3 values.
        
    The third argument specifies the type of the data which is GL_FLOAT(a vec * in GLSL consists of floating point values).
        
    The next argument specifies if we want the data to be normalized.
    If we're inputting integer data types (int, byte) and we've set this to GL_TRUE, the integer data is normalized to 0 (or -1 for signed data) and 1 when converted to float.
    This is not relevant for us so we'll leave this at GL_FALSE.
        
    The fifth argument is known as the stride and tells us the space between consecutive vertex attributes.
    Since the next set of position data is located exactly 3 times the size of a float away we specify that value as the stride.
    Note that since we know that the array is tightly packed(there is no space between the next vertex attribute value) 
    we could've also specified the stride as 0 to let OpenGL determine the stride (this only works when values are tightly packed). 
    Whenever we have more vertex attributes we have to carefully define the spacing between each vertex attribute but we'll get to see more examples of that later on.
        
    The last parameter is of type void*and thus requires that weird cast.This is the offset of where the position data begins in the buffer.
    Since the position data is at the start of the data array this value is just 0. We will explore this parameter in more detail later on
    */

    // ELEMENT BUFFER
    // create element buffer object
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // VERTEX ARRAY
    // create vertex array object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // RENDER LOOP
    while (!glfwWindowShouldClose(window))
    {
        // input
        //processInput(window);

        //##################################################################################################
        // rendering commands here:
        
        // At the start of frame we want to clear the screen. Otherwise we would still see the results from the previous frame
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // Right now we only care about the color values so we only clear the color buffer.
        glClear(GL_COLOR_BUFFER_BIT);
        // The possible bits we can set are GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BITand GL_STENCIL_BUFFER_BIT.
        
        // 5. draw the object
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //##################################################################################################
        
        // swap buffers and poll 
        glfwSwapBuffers(window);
        // IO events (keys pressed/released, mouse moved etc.)
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources
    glfwTerminate();
    return 0;
}

// NO
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_W && action == GLFW_PRESS){
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions
    glViewport(0, 0, width, height);
}
#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h"
#include "include/GLM/glm.hpp"
#include "include/GLM/ext.hpp"
#include <iostream>

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "lib/glew32.lib")
#pragma comment(lib, "lib/glfw3.lib")

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "shader.hpp"
#include "Skeleton.h"
#include "Animation.h"
#include "CmuFileParser.h"
#include "Simulator.h"

GLFWwindow* window;
GLuint vertexbuffer;
glm::mat4 view = glm::mat4(1.0f);
const int WINDOW_WITH = 1024;
const int WINDOW_HEIGHT = 728;

void glfwWindowInit() {
    if (!glfwInit()) {
        throw std::runtime_error("");
    }
    glfwWindowHint(GLFW_SAMPLES, 4); //
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //
    window = glfwCreateWindow(WINDOW_WITH, WINDOW_HEIGHT, "Scope", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("GLFW ");
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

void ft_glewInit() {
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("GLEW ");
    }
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f)); 
    if (glfwGetKey(window, GLFW_KEY_DOWN ) == GLFW_PRESS)
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, 1.0f));
    if (glfwGetKey(window, GLFW_KEY_RIGHT ) == GLFW_PRESS)
        view = glm::translate(view, glm::vec3(1.0f, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS)
        view = glm::translate(view, glm::vec3(-1.0f, 0.0f, 0.0f));
}

#include "include/GLM/gtx/string_cast.hpp"
int main() {

     
    try
    {
        glfwWindowInit();
        ft_glewInit();
        glfwSwapInterval(1);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
    glClearColor(0, 0, 1, 0);
    Shader shader;
    Skeleton skeleton;
    Animation animation;
    CmuFileParser parser("./test.asf",&skeleton, &animation);
    Simulator simulator(&skeleton, &animation);

    parser.loadCmuFile();
    shader.LoadShaders("C:/Users/schoe/Desktop/project_git/intern1/shaderSource/VertexShader", "C:/Users/schoe/Desktop/project_git/intern1/shaderSource/FragmentShader");
    glEnable(GL_PROGRAM_POINT_SIZE); 
    simulator.setInitPose();
    
    while (glfwWindowShouldClose(window) == 0)
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WITH / (float)WINDOW_HEIGHT, -0.01f, 15.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        skeleton.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
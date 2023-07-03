#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h"
#include "include/GLM/glm.hpp"
#include "include/GLM/ext.hpp"
#include "include/GLM/gtc/matrix_transform.hpp"
#include "include/GLM/gtx/transform.hpp"
#include <iostream>
#include <direct.h> // _getcwd
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "lib/glew32.lib")
#pragma comment(lib, "lib/glfw3.lib")

#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "include/shader.h"
#include "include/Skeleton.h"
#include "include/Animation.h"
#include "include/CmuFileParser.h"
#include "include/Simulator.h"
#include "include/AMCFileParser.h"

GLFWwindow* window;
glm::mat4 view = glm::mat4(1.0f);
//glm::mat4 view = glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
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
    if (glfwGetKey(window, GLFW_KEY_W ) == GLFW_PRESS)
        view = glm::translate(view, glm::vec3(0.0f, 1.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_S ) == GLFW_PRESS)
        view = glm::translate(view, glm::vec3(0.0f, -1.0f, 0.0f));
}

#include "include/GLM/gtx/string_cast.hpp"
int main() {

     
    try
    {
        glfwWindowInit();
        ft_glewInit();
        glfwSwapInterval(1);
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);
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
    CmuFileParser parser("./asf/131-dance.asf",&skeleton, &animation);
    AMCFileParser amcParser("./amc/131_04-dance.amc", &skeleton, &animation);
    
    assert(parser.loadCmuFile());
    assert(amcParser.loadAMCFile());

    std::string VertexShader(_getcwd(NULL,0));
    std::string FragmentShader(_getcwd(NULL, 0));

    VertexShader.append("/shaderSource/VertexShader.glsl");
    FragmentShader.append("/shaderSource/FragmentShader.glsl");
    shader.LoadShaders(VertexShader.c_str(), FragmentShader.c_str());


    glEnable(GL_PROGRAM_POINT_SIZE); 
    Simulator simulator(&skeleton, &animation);
    simulator.setupModelMatrix();
    simulator.animationDataMaping();
    
    uint32 animationDataIndex = 0;
    uint32 maxIndex = simulator.getTotalData();

    while (glfwWindowShouldClose(window) == 0 && animationDataIndex < maxIndex)
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WITH / (float)WINDOW_HEIGHT, -0.01f, 15.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        //shader.setUint("index", animationDataIndex++);
        simulator.draw(animationDataIndex++, shader._programId);
        //std::cout << animationDataIndex << '\n';
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
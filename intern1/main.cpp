#include "include/GLM/glm.hpp"
#include "include/GLM/ext.hpp"
#include "include/GLM/gtc/matrix_transform.hpp"
#include "include/GLM/gtx/transform.hpp"
#include <iostream>
#include <direct.h> // _getcwd
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "lib/glew32.lib")
#pragma comment(lib, "lib/glfw3.lib")
#include "include/common.h"
#include "include/Window.h"
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

#include "include/GLM/gtx/string_cast.hpp"

#include <chrono>
#include <cmath>
std::chrono::system_clock::time_point start;
std::chrono::duration<double>sec;
int main() 
{
    Window window;
    Shader shader("/shaderSource/VertexShader.glsl","/shaderSource/FragmentShader.glsl");
    
    window.initialize();
    shader.initialize();

    std::chrono::system_clock::time_point programstart = std::chrono::system_clock::now();
    Skeleton skeleton;
    Animation animation;
    CmuFileParser parser("./asf/131-dance.asf",&skeleton, &animation);
    AMCFileParser amcParser("./amc/131_04-dance.amc", &skeleton, &animation);
    
    start = std::chrono::system_clock::now();
    parser.loadCmuFile();
    sec = std::chrono::system_clock::now() - start;
    std::cout << "asf parsing time : " << sec.count() <<"seconds"<< std::endl;

    start = std::chrono::system_clock::now();
    amcParser.loadAMCFile();
    sec = std::chrono::system_clock::now() - start;
    std::cout << "amc parsing time : " << sec.count() <<"seconds"<< std::endl;

    Simulator simulator(&skeleton, &animation);
    start = std::chrono::system_clock::now();
    simulator.setupModelMatrix();
    sec = std::chrono::system_clock::now() - start;
    std::cout << "postion parsing time : " << sec.count() <<"seconds"<< std::endl;

    simulator.animationDataMaping();
    std::chrono::duration<double>programend = std::chrono::system_clock::now() - programstart;
    std::cout << "program time : " << programend.count() <<"seconds"<< std::endl;


    uint32 animationDataIndex = 0;
    uint32 maxIndex = simulator.getTotalData();

    while (window.isWindowClose() == false && animationDataIndex < maxIndex)
    {
        window.processInput();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)WINDOW_WITH / (float)WINDOW_HEIGHT, -0.01f, 5.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", window._view);
        simulator.draw(animationDataIndex++, shader._programId);
        window.bufferSwap();
        glfwPollEvents();
    }
}
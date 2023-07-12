#include "include/GLM/glm.hpp"
#include "include/GLFW/glfw3.h"
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "lib/glew32.lib")
#pragma comment(lib, "lib/glfw3.lib")
#include "include/Window.h"
#include "include/shader.h"
#include "include/Skeleton.h"
#include "include/Animation.h"
#include "include/CmuFileParser.h"
#include "include/Simulator.h"
#include "include/AMCFileParser.h"
#include "include/Ground.h"
#include <chrono>
#include <cmath>
std::chrono::system_clock::time_point start;
std::chrono::duration<double>sec;

void fileLoad(Simulator& simulator)
{
    std::vector<const char*> amcPathList = {"./amc/131_04-dance.amc", "./amc/07_05-walk.amc"};
    simulator._animations.resize(amcPathList.size());
    CmuFileParser parser(asfPath,&simulator._skeleton, &simulator._animations[0]);
    parser.loadCmuFile();

    AnimationData root = simulator._animations[0]._rootNode;
    for (int i = 0; i < simulator._animations.size(); ++i)
    {
        simulator._animations[i]._rootNode = root;
        AMCFileParser amcParser(amcPathList[i], &simulator._skeleton, &simulator._animations[i]);
        amcParser.loadAMCFile();
    }
}

int main() 
{
    Window window;
    Shader shader("/shaderSource/VertexShader.glsl","/shaderSource/FragmentShader.glsl");
    
    window.initialize();
    shader.initialize();

    Simulator simulator;
    fileLoad(simulator);
    simulator.initialize();

    uint32 animationDataIndex = 0;
    uint32 maxIndex = simulator.getTotalKeyCount();

    Ground ground;
    ground.initialize();
    while (window.isWindowClose() == false && animationDataIndex < maxIndex)
    {
        window.processInput();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)WINDOW_WITH / (float)WINDOW_HEIGHT, -0.01f, 5.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", window._view);
        simulator.draw(animationDataIndex++, shader._programId);
        ground.draw();
        if (animationDataIndex == maxIndex) animationDataIndex = 0;
        window.bufferSwap();
        glfwPollEvents();
    }
}
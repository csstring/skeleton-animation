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
#include "include/AnimationCompressor.h"
#include <chrono>
#include <cmath>
std::chrono::system_clock::time_point start;
std::chrono::duration<double>sec;
const float compressMul[] = {0 ,10.5, 94.6615, 355.184};
void fileLoad(Simulator& simulator)
{
    std::vector<const char*> amcPathList = {"./amc/walk1.amc", "./amc/run.amc","./amc/jump.amc","./amc/attack.amc","./amc/idle.amc", "./amc/dance.amc"};
    simulator._animations.push_back(Animation("walk", 1));
    simulator._animations.push_back(Animation("run", 1));
    simulator._animations.push_back(Animation("jump", 1));
    simulator._animations.push_back(Animation("attack", 1));
    simulator._animations.push_back(Animation("idle", 1));
    simulator._animations.push_back(Animation("dance", 1));
    CmuFileParser parser(asfPath,&simulator._skeleton, &simulator._animations[0]);
    parser.loadCmuFile();

    AnimationCompressor compressor;
    AnimationData root = simulator._animations[0]._rootNode;
    for (int i = 0; i < simulator._animations.size(); ++i)
    {
        simulator._animations[i]._rootNode = root;
        AMCFileParser amcParser(amcPathList[i], &simulator._skeleton, &simulator._animations[i]);
        amcParser.loadAMCFile();
        // if (simulator._animations[i]._name == "dance")
        //     compressor.CompressData(&simulator._animations[i], 355.184/100000000.0f);
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
    //비슷한 지점 찾기
    //simulator._animations[4].findSameFrame(10);
    Ground ground;
    ground.initialize();
    while (window.isWindowClose() == false)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)WINDOW_WITH / (float)WINDOW_HEIGHT, -0.01f, 5.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", window._view);
        
        window.processInput(simulator);
        simulator.draw();
        //ground.draw();
        window.bufferSwap();
        glfwPollEvents();
    }
}
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
int main() 
{
    Window window;
    Shader shader("/shaderSource/VertexShader.glsl","/shaderSource/FragmentShader.glsl");
    
    window.initialize();
    shader.initialize();

    Skeleton skeleton;
    Animation animation;
    CmuFileParser parser("./asf/131-dance.asf",&skeleton, &animation);
    AMCFileParser amcParser("./amc/131_04-dance.amc", &skeleton, &animation);
    
    assert(parser.loadCmuFile());
    assert(amcParser.loadAMCFile());

    Simulator simulator(&skeleton, &animation);
    simulator.setupModelMatrix();
    simulator.animationDataMaping();
    
    uint32 animationDataIndex = 0;
    uint32 maxIndex = simulator.getTotalData();

    while (window.isWindowClose() == false && animationDataIndex < maxIndex)
    {
        window.processInput();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)WINDOW_WITH / (float)WINDOW_HEIGHT, -0.01f, 30.0f);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        simulator.draw(animationDataIndex++, shader._programId);
        window.bufferSwap();
        glfwPollEvents();
    }
}
#pragma once
#include "Common.h"

struct GLFWwindow;
class Simulator;
class Camera;
class Window : Noncopyable
{
    public :
        explicit    Window(void) : _window(nullptr), _view(glm::mat4(1.0f)){}
                   ~Window(void){}
                   
        void        initialize(void);
        void        processInput(Simulator& simulator, Camera& camera);
        void        clearColorSetUp(float r = 0, float g = 0, float b = 0, float a = 0);
        bool        isWindowClose(void);
        void        bufferSwap(void);

        glm::mat4   _view;//fixme

    private :
        glm::mat4 createViewMatrix();
        GLFWwindow* _window;
};
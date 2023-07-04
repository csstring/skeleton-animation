#pragma once
#include "Common.h"

class GLFWwindow;
class Window : Noncopyable
{
    public :
        explicit    Window(void) : _window(nullptr), _view(glm::mat4(1.0f)){}
        void        initialize(void);
        void        processInput(void);
        void        glClearColor(float r = 0, float g = 0, float b = 0, float a = 0);
        bool        isWindowClose(void);
        void        bufferSwap(void);
        
    private :
        GLFWwindow* _window;
        glm::mat4   _view;
};
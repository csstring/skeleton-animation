#pragma once
#include "Common.h"

const int WINDOW_WITH = 1440;
const int WINDOW_HEIGHT = 960;
struct GLFWwindow;
class Simulator;
class Camera;
class Window : Noncopyable
{
    public :
        explicit    Window(void) : _window(nullptr){}
                   ~Window(void){}
                   
        void        initialize(void);
        void        processInput(Simulator& simulator, Camera& camera);
        void        clearColorSetUp(float r = 0, float g = 0, float b = 0, float a = 0);
        bool        isWindowClose(void);
        void        bufferSwap(void);
        void        framebuffer_size_callback(GLFWwindow* window, int width, int height);
    public :
        GLFWwindow* _window;

    private :
};
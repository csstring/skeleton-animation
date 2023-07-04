#include "include/Window.h"
#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"

void Window::initialize(void)
{
    if (glfwInit() == false) 
        ft_assert("glfwInit false");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    _window = glfwCreateWindow(WINDOW_WITH, WINDOW_HEIGHT, "Scope", NULL, NULL);  
    if (_window == NULL) 
    {
        glfwTerminate();
        ft_assert("glfw create Window failed");
    }
    glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwMakeContextCurrent(_window);
    if (glewInit() != GLEW_OK) 
        ft_assert("glew init failed");

    this->glClearColor();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDepthFunc(GL_LESS);
}

void Window::processInput(void)
{
    if(glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(_window, true);
    if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(0.0f, 0.0f, -1.0f)); 
    if (glfwGetKey(_window, GLFW_KEY_DOWN ) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(0.0f, 0.0f, 1.0f));
    if (glfwGetKey(_window, GLFW_KEY_RIGHT ) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(1.0f, 0.0f, 0.0f));
    if (glfwGetKey(_window, GLFW_KEY_LEFT ) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(-1.0f, 0.0f, 0.0f));
    if (glfwGetKey(_window, GLFW_KEY_W ) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(0.0f, 1.0f, 0.0f));
    if (glfwGetKey(_window, GLFW_KEY_S ) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(0.0f, -1.0f, 0.0f));
}

void Window::glClearColor(float r = 0, float g = 0, float b = 0, float a = 0)
{
    glClearColor(a,g,b,a);
}

bool Window::isWindowClose(void)
{
    if (glfwWindowShouldClose(_window) == 0)
        return false;
    return true;
}

void Window::bufferSwap(void)
{
    glfwSwapBuffers(_window);
}
#include "include/GLM/ext.hpp"
#include "include/Window.h"
#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"
#include "include/Simulator.h"

void Window::initialize(void)
{
    if (glfwInit() == false) 
        ft_assert("glfwInit false");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    _window = glfwCreateWindow(WINDOW_WITH, WINDOW_HEIGHT, "model", NULL, NULL);  
    if (_window == NULL) 
    {
        glfwTerminate();
        ft_assert("glfw create Window failed");
    }
    glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);

    glfwMakeContextCurrent(_window);
    if (glewInit() != GLEW_OK) 
        ft_assert("glew init failed");

    clearColorSetUp();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDepthFunc(GL_LESS);

    _view = createViewMatrix();
}

void Window::processInput(Simulator& simulator)
{
    if(glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(_window, true);
    //z축
    // if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS)
    //     _view = glm::translate(_view, glm::vec3(0.0f, -0.0f, -0.3f)); 
    // if (glfwGetKey(_window, GLFW_KEY_DOWN ) == GLFW_PRESS)
    //     _view = glm::translate(_view, glm::vec3(0.0f, 0.0f, 0.3f));
    // if (glfwGetKey(_window, GLFW_KEY_W ) == GLFW_PRESS)
    //     _view = glm::translate(_view, glm::vec3(0.0f, 0.3f, 0.0f));
    // if (glfwGetKey(_window, GLFW_KEY_S ) == GLFW_PRESS)
    //     _view = glm::translate(_view, glm::vec3(0.0f, -0.3f, 0.0f));
    
    //y축
    if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(0.0f, -0.5f, -0.0f)); 
    if (glfwGetKey(_window, GLFW_KEY_DOWN ) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(0.0f, 0.5f, 0.0f));
    if (glfwGetKey(_window, GLFW_KEY_W ) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(0.0f, 0.0f, -0.5f));
    if (glfwGetKey(_window, GLFW_KEY_S ) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(0.0f, -0.0f, 0.5f));

    if (glfwGetKey(_window, GLFW_KEY_RIGHT ) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(0.3f, 0.0f, 0.0f));
    if (glfwGetKey(_window, GLFW_KEY_LEFT ) == GLFW_PRESS)
        _view = glm::translate(_view, glm::vec3(-0.3f, 0.0f, 0.0f));
    if (glfwGetKey(_window, GLFW_KEY_KP_8 ) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::UP);
    if (glfwGetKey(_window, GLFW_KEY_KP_5 ) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::BACK);
    if (glfwGetKey(_window, GLFW_KEY_KP_4 ) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::REFT);
    if (glfwGetKey(_window, GLFW_KEY_KP_6 ) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::RIGHT);
    if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::ATTACK);
    if (glfwGetKey(_window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::JUMP);
    if (glfwGetKey(_window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::RUN);
}

void Window::clearColorSetUp(float r, float g, float b, float a)
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

glm::mat4 Window::createViewMatrix()
{
    //y
    glm::vec3 cameraPos(0.0f, 100.0f, 0.0f);  // 카메라 위치
    glm::vec3 targetPos(0.0f, 0.0f, 0.0f);   // 바라보는 지점
    glm::vec3 upVector(0.0f, 0.0f, 1.0f);    // 위쪽 방향
    //z
    // glm::vec3 cameraPos(0.0f, 0.0f, 100.0f);  // 카메라 위치
    // glm::vec3 targetPos(0.0f, 0.0f, 0.0f);   // 바라보는 지점
    // glm::vec3 upVector(0.0f, 1.0f, 0.0f);    // 위쪽 방향
    glm::mat4 viewMatrix = glm::lookAt(cameraPos, targetPos, upVector);
    return viewMatrix;
}
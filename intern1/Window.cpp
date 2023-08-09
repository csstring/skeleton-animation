#include "include/GLM/ext.hpp"
#include "include/Window.h"
#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"
#include "include/Simulator.h"
#include "include/Camera.h"
#include "include/EnumHeader.h"

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
    glfwSwapInterval(1);

}

void Window::processInput(Simulator& simulator, Camera& camera)
{
    static int currentWalkState,previousWalkState;
    static int currentBackState,previousBackState;
    static int currentRunState,previousRunState;
    static int currentAddCharState,previousAddCharState;
    static int currentCharChangeState,previousCharChangeState;

    currentWalkState = glfwGetKey(_window, GLFW_KEY_KP_8);
    currentBackState = glfwGetKey(_window, GLFW_KEY_KP_5);
    currentRunState = glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT);
    currentAddCharState = glfwGetKey(_window, GLFW_KEY_P);
    currentCharChangeState = glfwGetKey(_window, GLFW_KEY_SPACE);

    if(glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(_window, true);

    if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
        camera._cameraPos += cameraSpeed * camera._cameraFront;
    if (glfwGetKey(_window, GLFW_KEY_S ) == GLFW_PRESS)
        camera._cameraPos -= cameraSpeed * camera._cameraFront;
    if (glfwGetKey(_window, GLFW_KEY_A ) == GLFW_PRESS)
        camera._cameraPos -= camera._cameraRight * cameraSpeed;
    if (glfwGetKey(_window, GLFW_KEY_D ) == GLFW_PRESS)
        camera._cameraPos += camera._cameraRight * cameraSpeed;

    if (currentWalkState == GLFW_PRESS && previousWalkState == GLFW_RELEASE)
    {
        simulator.changeAnimation(KeyInput::UP);
        previousWalkState = currentWalkState;
    } else if (currentWalkState == GLFW_RELEASE && previousWalkState == GLFW_PRESS) {
        previousWalkState = currentWalkState;
        simulator.changeAnimation(KeyInput::STOP);
    }
    if (currentBackState == GLFW_PRESS && previousBackState == GLFW_RELEASE)
    {
        previousBackState = currentBackState;
    } else if (currentBackState == GLFW_RELEASE && previousBackState == GLFW_PRESS) {
        previousBackState = currentBackState;
        simulator.changeAnimation(KeyInput::LOWERBACK);
    }
    if (glfwGetKey(_window, GLFW_KEY_KP_4 ) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::REFT);
    if (glfwGetKey(_window, GLFW_KEY_KP_6 ) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::RIGHT);
    if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::ATTACK);
    if (glfwGetKey(_window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS || glfwGetKey(_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::JUMP);
    if (currentRunState == GLFW_PRESS && previousRunState == GLFW_RELEASE)
    {
        simulator.changeAnimation(KeyInput::RUN);
        previousRunState = currentRunState;
    } else if (currentRunState == GLFW_RELEASE && previousRunState == GLFW_PRESS) {
        previousRunState = currentRunState;
    }
    if (glfwGetKey(_window, GLFW_KEY_Z ) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::DRINK);
    if (glfwGetKey(_window, GLFW_KEY_X ) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::ROLL);
    if (glfwGetKey(_window, GLFW_KEY_C ) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::GOLF);
    
    if (glfwGetKey(_window, GLFW_KEY_U) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::CUBEFRONT);
    if (glfwGetKey(_window, GLFW_KEY_J) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::CUBEBACK);
    if (glfwGetKey(_window, GLFW_KEY_H) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::CUBERIGHT);
    if (glfwGetKey(_window, GLFW_KEY_K) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::CUBELEFT);
    if (glfwGetKey(_window, GLFW_KEY_Y) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::CUBEUP);
    if (glfwGetKey(_window, GLFW_KEY_I) == GLFW_PRESS)
        simulator.changeAnimation(KeyInput::CUBEDOWN);

    if (currentAddCharState == GLFW_PRESS && previousAddCharState == GLFW_RELEASE)
    {
        simulator.addPlayer("idle");
        previousAddCharState = currentAddCharState;
    } else if (currentAddCharState == GLFW_RELEASE && previousAddCharState == GLFW_PRESS) {
        previousAddCharState = currentAddCharState;
    }
    if (currentCharChangeState == GLFW_PRESS && previousCharChangeState == GLFW_RELEASE)
    {
        simulator.changeControllCharacter();
        previousCharChangeState = currentCharChangeState;
    } else if (currentCharChangeState == GLFW_RELEASE && previousCharChangeState == GLFW_PRESS) {
        previousCharChangeState = currentCharChangeState;
    }
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
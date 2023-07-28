#include "include/Camera.h"

void Camera::update(void)
{
    _view = glm::lookAt(_cameraPos, _cameraPos+_cameraFront, _cameraUp);
}

void Camera::initialize(void)
{
    _cameraPos = glm::vec3(0.0f,3.0f,50.0f);
    _cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
    _cameraUp = glm::vec3(0.0f, 1.0f,0.0f);
}
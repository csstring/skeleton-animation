#pragma once
#include "include/Common.h"

const float cameraSpeed = 0.5f;
class Camera
{
    public:
        glm::mat4   _view;
        glm::vec3   _cameraPos;
        glm::vec3   _cameraFront;
        glm::vec3   _cameraUp;

    public:
        Camera(){};
        ~Camera(){};
        void update(void);
        void initialize(void);
};  

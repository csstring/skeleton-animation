#pragma once
#include "Common.h"

const float cameraSpeed = 0.5f;
class Camera
{

    public:
        Camera() : _yaw(-90.0f), _pitch(0.0f), _movementSpeed(2.5f), _mouseSensitivity(0.1f), _zoom(45.0f), _isFirst(true){};
        ~Camera(){};
        void update(void);
        void initialize(void);
        void ProcessMouseScroll(float yoffset);
        void updateCameraVectors(void);
        void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch);
        void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    private:
        float _yaw;
        float _pitch;
        float _movementSpeed;
        float _mouseSensitivity;
        float _zoom;
        glm::vec3   _cameraUp;
        bool _isFirst;
        float _lastX;
        float _lastY;
    public:
        glm::vec3   _cameraPos;
        glm::vec3   _cameraFront;
        glm::vec3   _cameraRight;
        glm::mat4   _view;
};  
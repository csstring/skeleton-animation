#pragma once
#include "include/Common.h"
class Bone;

class EyeIK
{
    private:
        bool _targetOn;
        glm::vec3 _targetPosition;
        std::vector<glm::vec3> _bonedirection;
        std::vector<uint8> _eyeBoneIndex;
        std::vector<glm::mat4> _IKTranspos;
        bool reachable(const std::vector<glm::mat4>& characterTranspos, std::vector<float>& distance);
    
    public:
        std::chrono::steady_clock::time_point _start;
    
    public:
        EyeIK()
        {
            _targetOn = false;
        };
        ~EyeIK(){};

        void initialize(const std::vector<Bone>& boneVector);
        void setTargetPosition(glm::vec3 targetPosition);
        bool targetPositionCheck(const std::vector<glm::mat4>& characterTranspos);
        const std::vector<glm::mat4>& solveEyeIK(const std::vector<glm::mat4>& characterTranspos);
};

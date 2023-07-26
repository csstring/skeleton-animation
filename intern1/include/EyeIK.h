#pragma once
#include "Common.h"
#include "BoneLocal.h"
class Bone;
class Controller;
class EyeIK
{
    private:
        bool _targetOn;
        glm::vec3 _targetPosition;
        std::vector<glm::vec3> _bonedirection;
        std::vector<uint8> _eyeBoneIndex;
        std::vector<glm::mat4> _IKTranspos;
        const std::vector<Bone>& _boneVector;//지워야 할듯?
    private:
        bool reachable(const std::vector<glm::mat4>& characterTranspos, std::vector<float>& distance,glm::vec3 headPosition);
        void positionFixLimitAngle(glm::vec3& start, glm::vec3& end, const glm::quat& startBonedir, const Bone& endBone,bool backOrFront);
    public:
        std::chrono::steady_clock::time_point _start;
    
    public:
        explicit EyeIK(const std::vector<Bone>& boneVector) : _boneVector(boneVector)
        {
            _targetOn = false;
        };
        ~EyeIK(){};

        void initialize();
        void setTargetPosition(glm::vec3 targetPosition);
        bool targetPositionCheck(const std::vector<glm::mat4>& characterTranspos);
        const std::vector<glm::mat4>& solveEyeIK(
            std::vector<BoneLocal>& _boneLocalVector, 
            const glm::mat4& worldRotation, 
            const glm::mat4& worldTranslate,
            const Controller& _controller
        );
};

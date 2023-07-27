#pragma once
#include "Common.h"
#include "BoneLocal.h"
class Bone;
class Controller;
const float LOWERNECK_RATIO = 0.2;
const float UPPER_RATIO = 0.35;
const float HEAD_RATIO = 0.45;
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
        glm::vec3 moveInBoneLocalPos(const glm::vec3& start, const glm::vec3& end, const glm::quat& toTargetDir, const glm::vec3& endBoneDir, float ratio);
        bool limitAngleCheck(const Bone& bone, const glm::quat& boneRot);
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
        void solveEyeIK(
            std::vector<BoneLocal>& _boneLocalVector, 
            const glm::mat4& worldRotation, 
            const glm::mat4& worldTranslate,
            const Controller& _controller
        );
};

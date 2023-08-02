#pragma once
#include "../Common.h"
#include "../BoneLocal.h"
class Bone;
class Controller;
const float LOWERNECK_RATIO = 0.2;
const float UPPER_RATIO = 0.35;
const float HEAD_RATIO = 1 - LOWERNECK_RATIO - UPPER_RATIO;
class EyeIK
{
    private:
        bool                     _targetOn;
        bool                     _isFirst;
        float                    _blendingRatio;
        glm::vec3                _targetPosition;
        std::vector<glm::vec3>   _bonedirection;
        std::vector<uint8>       _eyeBoneIndex;
        const std::vector<Bone>& _boneVector;//지워야 할듯?
    private:
        glm::vec3 moveInBoneLocalPos(const glm::vec3& start, const glm::vec3& end, const glm::quat& toTargetDir, const glm::vec3& endBoneDir, float ratio);
        void blendingRatioUpdate(const std::chrono::steady_clock::time_point& curTime);

    public:
        std::chrono::steady_clock::time_point _callTime;
    
    public:
        explicit EyeIK(const std::vector<Bone>& boneVector) : _boneVector(boneVector)
        {
            _targetOn = false;
            _isFirst = true;
            _blendingRatio = 0;
        };
        ~EyeIK(){};

        void initialize();
        void setTargetPosition(glm::vec3 targetPosition);
        void solveEyeIK(
            std::vector<BoneLocal>& _boneLocalVector, 
            const glm::mat4& worldRotation, 
            const glm::mat4& worldTranslate,
            const Controller& _controller,
            const std::chrono::steady_clock::time_point& curTime
        );
};

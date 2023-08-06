#pragma once
#include "../Common.h"
#include "../BoneLocal.h"
#include "IKInterface.h"

const float LOWERNECK_RATIO = 0.2;
const float UPPER_RATIO = 0.35;
const float HEAD_RATIO = 1 - LOWERNECK_RATIO - UPPER_RATIO;
class EyeIK : public IKInterface
{
    private:
        glm::vec3 moveInBoneLocalPos(const glm::vec3& start, const glm::vec3& end, const glm::quat& toTargetDir, const glm::vec3& endBoneDir, float ratio);
        void blendingRatioUpdate(const std::chrono::steady_clock::time_point& curTime);
    
    public:
        explicit EyeIK(const std::vector<Bone>& boneVector) : IKInterface(boneVector){};
        ~EyeIK(){};

        virtual void solveIK(
            std::vector<BoneLocal>& _boneLocalVector, 
            const glm::mat4& worldRotation, 
            const glm::mat4& worldTranslate,
            const Controller& _controller,
            const std::chrono::steady_clock::time_point& curTime
        ) override final;
};

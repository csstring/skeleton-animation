#pragma once
#include "../Common.h"
#include "../BoneLocal.h"
#include "IKInterface.h"

class FootIK : public IKInterface
{
    private:
        glm::vec3 _groundNormal;
    public:
        explicit FootIK(const std::vector<Bone>& boneVector) : IKInterface(boneVector){};
        ~FootIK(){};
        void findGround(void);
        void setGroundNormal(glm::vec3);
        virtual void solveIK(
            std::vector<BoneLocal>& _boneLocalVector, 
            const glm::mat4& worldRotation, 
            const glm::mat4& worldTranslate,
            const Controller& _controller,
            const std::chrono::steady_clock::time_point& curTime
        ) override final;
};

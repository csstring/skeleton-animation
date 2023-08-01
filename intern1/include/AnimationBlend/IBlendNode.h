#pragma once
#include "../Common.h"
#include "../TimeNode.h"
#include "../Animation.h"
#include <deque>
#include "../EnumHeader.h"

struct BoneLocal;
class IBlendNode
{
    private:
    public:
        IBlendNode(){};
        virtual ~IBlendNode(){};

        void getTransFormByKeyFrame(glm::quat& interpolR, glm::vec3& interpolT, const AnimationData* node, uint32 keyFrame);
        void updateTransForm(const AnimationData& node, uint32 keyFrame, float interpolVal, std::vector<BoneLocal>& _boneLocalVector, std::vector<BONEID> boneIds);
        void changeUpperState(UpperState& upperState, const std::string& name);
        void changeLowerState(LowerState& loswerState, const std::string& name);
        virtual void eraseAnimation(const std::chrono::steady_clock::time_point& curTime) = 0;
        virtual void update(
            const std::chrono::steady_clock::time_point& curTime, 
            std::vector<BoneLocal>& _boneLocalVector,
            LowerState& lowerState,
            UpperState& upperState ) = 0;
    
    public: 
        std::deque<std::pair<const Animation*, TimeNode>> _animations;
};
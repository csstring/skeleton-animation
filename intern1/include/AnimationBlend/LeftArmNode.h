#pragma once
#include "../Common.h"
#include "IBlendNode.h"

class BoneLocal;
class LeftArmNode : public IBlendNode
{
    public:
        LeftArmNode(){};
        virtual ~LeftArmNode(){};

        virtual void eraseAnimation(const std::chrono::steady_clock::time_point& curTime) override final;
        virtual void update(
            const std::chrono::steady_clock::time_point& curTime, 
            std::vector<BoneLocal>& _boneLocalVector,
            LowerState& lowerState,
            UpperState& upperState 
        ) override final;
};
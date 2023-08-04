#pragma once
#include "../Common.h"
#include "IBlendNode.h"

class BoneLocal;
class LowerNode : public IBlendNode
{
    public:
        LowerNode(BlendNode nodeNum):IBlendNode(nodeNum){};
        virtual ~LowerNode(){};

        virtual void eraseAnimation(const std::chrono::steady_clock::time_point& curTime) override final;
        virtual void update(
            const std::chrono::steady_clock::time_point& curTime, 
            std::vector<BoneLocal>& _boneLocalVector,
            LowerState& lowerState,
            UpperState& upperState 
        ) override final;
};
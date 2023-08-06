#pragma once
#include "../Common.h"
#include "IBlendNode.h"

class BoneLocal;
class UpperNode : public IBlendNode
{
    public:
        UpperNode(BlendNode nodeNum):IBlendNode(nodeNum){};
        virtual ~UpperNode(){};

        virtual void eraseAnimation(const std::chrono::steady_clock::time_point& curTime) override final;
        virtual void update(
            const std::chrono::steady_clock::time_point& curTime, 
            std::vector<BoneLocal>& _boneLocalVector,
            LowerState& lowerState,
            UpperState& upperState 
        ) override final;
};
#pragma once
#include "../Common.h"
#include "../TimeNode.h"
#include "../Animation.h"
#include <deque>
#include "../EnumHeader.h"

struct BoneLocal;
class IBlendNode
{
    public:
        IBlendNode(BlendNode nodeNum): _nodeNum(nodeNum)
        {
            if (nodeNum == BlendNode::BASE)
                _parentIndex = -1;
            if (nodeNum == BlendNode::UPPER)
                _parentIndex = 0;
            if (nodeNum == BlendNode::LOWER)
                _parentIndex = 0;
            if (nodeNum == BlendNode::LEFTARM)
                _parentIndex = 1;
        };
        virtual ~IBlendNode(){};

        void getTransFormByKeyFrame(glm::quat& interpolR, glm::vec3& interpolT, const AnimationData* node, uint32 keyFrame);
        void updateTransForm(const AnimationData& node, uint32 keyFrame, float interpolVal, std::vector<BoneLocal>& _boneLocalVector, std::vector<BONEID> boneIds, BlendNode nodeNum);
        void changeUpperState(UpperState& upperState, const std::string& name);
        void changeLowerState(LowerState& loswerState, const std::string& name);
        virtual void eraseAnimation(const std::chrono::steady_clock::time_point& curTime) = 0;
        virtual void update(
            const std::chrono::steady_clock::time_point& curTime, 
            std::vector<BoneLocal>& _boneLocalVector,
            LowerState& lowerState,
            UpperState& upperState) = 0;
        
    public:
        const BlendNode _nodeNum;
        int32           _parentIndex;
        uint32          _state;
        std::deque<std::pair<const Animation*, TimeNode>> _animations;
};
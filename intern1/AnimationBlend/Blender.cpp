#include "../include/AnimationBlend/Blender.h"
#include "../include/AnimationBlend/BaseNode.h"
#include "../include/AnimationBlend/UpperNode.h"
#include "../include/AnimationBlend/LowerNode.h"
#include "../include/AnimationBlend/LeftArmNode.h"
#include "../include/BoneLocal.h"
#include "../include/Skeleton.h"
#include "../include/IK/IKUtility.h"

IBlendNode* Blender::getBlendNode(BlendNode nodeNum)
{
    for (const auto& node : _blendTree)
    {
        if (node->_nodeNum == nodeNum)
            return node;
    }
    return nullptr;
}

void Blender::eraseAnimationCall(const std::chrono::steady_clock::time_point& curTime)
{
    for (const auto& node : _blendTree)
        node->eraseAnimation(curTime);
}

void Blender::initialize(void)
{
    _blendTree.push_back(new BaseNode(BlendNode::BASE));
    _blendTree.push_back(new UpperNode(BlendNode::UPPER));
    _blendTree.push_back(new LowerNode(BlendNode::LOWER));
    _blendTree.push_back(new LeftArmNode(BlendNode::LEFTARM));
}

void Blender::animationUpdate(
    const std::chrono::steady_clock::time_point& curTime, 
    std::vector<BoneLocal>& boneLocalVector,
    LowerState& lowerState,
    UpperState& upperState
)
{
    for (const auto& node : _blendTree)
    {
        node->update(curTime, boneLocalVector, lowerState, upperState);
        if (node->_parentIndex >= 1 && _blendTree[node->_parentIndex]->_state > node->_state)
        {
            node->_animations.clear();
            node->_state = 0;
        }
    }
}
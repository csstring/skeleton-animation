#include "../include/AnimationBlend/Blender.h"
#include "../include/AnimationBlend/BaseNode.h"
#include "../include/AnimationBlend/UpperNode.h"
#include "../include/AnimationBlend/LowerNode.h"
#include "../include/BoneLocal.h"

IBlendNode* Blender::getBlendNode(BlendNode nodeNum)
{
    for (const auto& node : _blendTree)
    {
        if (node.first == nodeNum)
            return node.second;
    }
    return nullptr;
}

void Blender::eraseAnimationCall(const std::chrono::steady_clock::time_point& curTime)
{
    for (const auto& node : _blendTree)
        node.second->eraseAnimation(curTime);
}


void Blender::initialize(void)
{
    _blendTree.push_back({BlendNode::BASE, new BaseNode()});
    _blendTree.push_back({BlendNode::LOWER, new LowerNode()});
    _blendTree.push_back({BlendNode::UPPER, new UpperNode()});
}

void Blender::animationUpdate(const std::chrono::steady_clock::time_point& curTime, std::vector<BoneLocal>& boneLocalVector)
{
    for (const auto& node : _blendTree)
        node.second->update(curTime, boneLocalVector);
}
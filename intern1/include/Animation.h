#pragma once
#include "common.h"
#include <vector>
#include "GLM/glm.hpp"

class AnimationTreeTraversal;
struct AnimationData
{
        std::vector<glm::quat> _localRotation;
        std::vector<glm::mat4> _worldTrans;
        std::vector<glm::vec4> _localTrans;
        std::vector<glm::vec4> _pos;
        glm::quat _c;
        glm::quat _invC;
        glm::vec4 _b;
        int32      _boneIndex;
        AnimationData*      _parentPointer;
        std::vector<AnimationData> _childrens;
};

class Animation : Noncopyable
{
public:
    AnimationData _rootNode;
    Animation()
    {
        _rootNode._boneIndex = 0;
        _rootNode._parentPointer = NULL;
    };
    ~Animation(){};

    AnimationData* returnAnimationData(const uint32 boneIndex);
    void AnimationDataTraver(AnimationTreeTraversal& travel);
};
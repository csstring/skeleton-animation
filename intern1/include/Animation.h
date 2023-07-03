#pragma once
#include "common.h"
#include <vector>
#include "GLM/glm.hpp"

class AnimationTreeTraversal;
struct AnimationData
{
        std::string _name;
        std::vector<glm::mat4> _matrix;
        std::vector<glm::vec4> _pos;
        glm::mat4 __c;//fix
        glm::mat4 __b;//fix
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
        _rootNode._name = "root";
        _rootNode._boneIndex = 0;
        _rootNode._parentPointer = NULL;
    };
    ~Animation(){};

    AnimationData* returnAnimationData(const std::string& name);
    void AnimationDataTraver(AnimationTreeTraversal& travel);
};
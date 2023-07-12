#pragma once
#include "common.h"
#include <utility>
#include <vector>
#include "include/GLM/ext.hpp"

class AnimationTreeTraversal;

struct AnimationData
{
        std::vector<std::pair<float,glm::quat>>     _localRotation;
        std::vector<std::pair<float,glm::mat4>>     _localTrans;

        int32                                       _boneIndex;
        std::vector<AnimationData>                  _childrens;
};

class Animation//복사해서 비교 데이터 만들어야 할듯
{
    public:
        explicit Animation()
        {
            _rootNode._boneIndex = 0;
        }
        ~Animation(){}

        AnimationData* returnAnimationData(const uint32 boneIndex);
        void           AnimationDataTraver(AnimationTreeTraversal& travel);
        
    public:
        AnimationData  _rootNode;
        std::string    _name;
};
#pragma once
#include "common.h"
#include <utility>
#include <vector>
#include "GLM/ext.hpp"

class AnimationTreeTraversal;

struct AnimationData
{
        std::vector<std::pair<uint32,glm::quat>>     _localRotation;
        std::vector<std::pair<uint32,glm::vec3>>     _localTrans;

        int32                                       _boneIndex;
        std::vector<AnimationData>                  _childrens;
};

class Animation
{
    private:
        void getDataNode(const uint32 boneIndex, AnimationData& node, AnimationData** returnNode) const;
        void getDataNode(const uint32 boneIndex, const AnimationData& node, const AnimationData** returnNode) const;
    public:
        explicit Animation(const char* name, float animationSpeed) : _name(name), _animationSpeed(animationSpeed)
        {
            _rootNode._boneIndex = 0;
        }
        ~Animation(){}

        AnimationData* returnAnimationData(const uint32 boneIndex);
        const AnimationData* returnAnimationData(const uint32 boneIndex) const;
        void           AnimationDataTraver(AnimationTreeTraversal& travel);
        void           findSameFrame(uint32 count);
    public:
        uint64         _animationMillisecond;
        float          _animationSpeed;
        AnimationData  _rootNode;
        std::string    _name;
};
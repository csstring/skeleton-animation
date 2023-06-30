#pragma once
#include "common.h"

struct AnimationData
{
    std::string _name;
    int32      _boneIndex;
    int32      _parentIndex;
    std::vector<AnimationData> _childrens;
};

class Animation : Noncopyable
{
private:
public:
    AnimationData _rootNode;
    Animation()
    {
        _rootNode._name = "root";
        _rootNode._boneIndex = 0;
        _rootNode._parentIndex = -1;
    };
    ~Animation(){};

    AnimationData* returnAnimationData(const std::string& name);
};
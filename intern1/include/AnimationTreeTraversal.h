#pragma once

struct AnimationData;
class AnimationTreeTraversal
{
public:
    virtual void processNode(AnimationData& data) = 0;
    virtual ~AnimationTreeTraversal(){};
};
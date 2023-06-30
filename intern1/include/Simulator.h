#pragma once
#include "common.h"
#include "Bone.h"

class Animation;
class Skeleton;
class AnimationData;

class Simulator : Noncopyable
{
private:
    Skeleton* _skeleton;
    const Animation* _animation;
    
    void setPoseDfs(std::vector<Bone>& boneVector, const AnimationData& node);
public:
    Simulator(Skeleton* skeleton, Animation* animation) : _skeleton(skeleton), _animation(animation)
    {}
    ~Simulator(){}

    void setInitPose(void);
};
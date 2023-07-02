#pragma once
#include "common.h"
#include "Bone.h"
#include <vector>

class Animation;
class Skeleton;
class AnimationData;

class Simulator : Noncopyable
{
private:
    Skeleton*           _skeleton;
    Animation*          _animation;
    std::vector<uint32> VAO, VBO;

    void setPoseDfs(std::vector<Bone>& boneVector, const AnimationData& node);
public:
    Simulator(Skeleton* skeleton, Animation* animation) : _skeleton(skeleton), _animation(animation)
    {
        VAO.resize(_skeleton->getBoneVector().size());
        VBO.resize(_skeleton->getBoneVector().size());
    }
    ~Simulator(){}

    void setupModelMatrix(void);
    void animationDataMaping(void);
    void draw(void) const;
};
#pragma once
#include "common.h"
#include "Bone.h"
#include <vector>
#include "Skeleton.h"
#include <iostream>
class Animation;
struct AnimationData;

class Simulator : Noncopyable
{
private:
    Skeleton*           _skeleton;
    Animation*          _animation;
    std::vector<uint32> VAO, VBO;

public:
    Simulator(Skeleton* skeleton, Animation* animation) : _skeleton(skeleton), _animation(animation)
    {
        //std::cout << skeleton->getBoneVector().size() << std::endl;
        VAO.resize(_skeleton->getBoneVector().size());
        VBO.resize(_skeleton->getBoneVector().size());
        //std::cout << _skeleton->getBoneVector().size() << std::endl;
    }
    ~Simulator(){}

    void setupModelMatrix(void);
    void animationDataMaping(void);
    void draw(void) const;
};
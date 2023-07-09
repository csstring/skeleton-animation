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
        uint32              _total;
    public:
        Simulator(Skeleton* skeleton, Animation* animation) : _skeleton(skeleton), _animation(animation){}
        ~Simulator(){}
        void initialize(void);

        //getter
        uint32 getTotalData(void){return _total;};

        void setupModelMatrix(void);
        void animationDataMaping(void);
        void draw(uint32 animationTime,uint32 shaderPrograms);
};
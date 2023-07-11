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
        Skeleton*              _skeleton;
        Animation*             _animation;
        std::vector<uint32>    VAO, VBO, VBC;
        uint32                 _keyCount;
        std::vector<glm::mat4> _transForm;
        std::vector<uint32>    _compresskeyFrame[4];//애니메이션 개수 만큼 배열 되어야 할듯

        void updateTransForm(const AnimationData& node, glm::mat4 wolrdTrans, float* keyArray);

    public:
        Simulator(Skeleton* skeleton, Animation* animation) : _skeleton(skeleton), _animation(animation){}
        ~Simulator(){}
        void initialize(void);

        //getter
        uint32 getTotalKeyCount(void){return _keyCount;};

        void setupModelMatrix(void);
        void animationDataMaping(void);
        void update(uint32 keyTime, uint32 animationIndex);
        void draw(uint32 animationTime,uint32 shaderPrograms);
};
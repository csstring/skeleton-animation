#pragma once
#include "common.h"
#include "Bone.h"
#include "Skeleton.h"
#include <vector>
#include <iostream>
#include "Animation.h"
struct AnimationData;

class Simulator : Noncopyable
{
    private:
        std::vector<uint32>    VAO, VBO, VBC;
        uint32                 _keyCount;
        glm::mat4              _modelPos;
        std::vector<glm::mat4> _transForm;
        std::vector<uint32>    _compresskeyFrame[4];//애니메이션 개수 만큼 배열 되어야 할듯

        void updateTransForm(const AnimationData& node, glm::mat4 wolrdTrans, float* keyArray);

    public:
        Simulator()
        {
            _modelPos = glm::mat4(1.0f);
        }
        ~Simulator(){}
        void initialize(void);

        //getter
        uint32 getTotalKeyCount(void){return _keyCount;};

        void boneBufferMaping(void);
        void update(uint32 keyTime, uint32 animationIndex);
        void draw(uint32 animationTime,uint32 shaderPrograms);

    public : 
        Skeleton               _skeleton;
        std::vector<Animation> _animations;
};
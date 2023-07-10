#pragma once
#include "common.h"
#include <vector>
#include "include/GLM/ext.hpp"

class AnimationTreeTraversal;

struct AnimationData
{
        std::vector<glm::quat>     _localRotation;
        //복사 애니메이션 객체 따로 만들고 worldTrans,index 넣어도 되지 않나
        std::vector<glm::vec4>     _localTrans;

        // std::vector<glm::mat4>     _worldTrans;
        // glm::quat                  _c;
        // glm::quat                  _invC;
        // glm::vec4                  _b; //amc에서 local정하기
        int32                      _boneIndex;
        AnimationData*             _parentPointer;
        std::vector<AnimationData> _childrens;
};

class Animation : Noncopyable//복사해서 비교 데이터 만들어야 할듯
{
    public:
        explicit Animation()
        {
            _rootNode._boneIndex = 0;
            _rootNode._parentPointer = NULL;
        };
        ~Animation(){};

        AnimationData* returnAnimationData(const uint32 boneIndex);
        void           AnimationDataTraver(AnimationTreeTraversal& travel);
        
    public:
        AnimationData  _rootNode;

};
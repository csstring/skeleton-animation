#pragma once
#include "../EnumHeader.h"
#include "IBlendNode.h"

struct BoneLocal;
class Animation;
class Skeleton;

class Blender
{
    private:
        std::vector<IBlendNode*> _blendTree;
    public:
        Blender(){};
        ~Blender(){};
        void initialize(void);
        IBlendNode* getBlendNode(BlendNode nodeNum);
        void animationUpdate(
            const std::chrono::steady_clock::time_point& curTime, 
            std::vector<BoneLocal>& boneLocalVector,
            LowerState& lowerState,
            UpperState& upperState
        );
        void eraseAnimationCall(const std::chrono::steady_clock::time_point& curTime);
        void angleCheck(const Skeleton& skeleton, std::vector<BoneLocal>& boneLocalVector);
};
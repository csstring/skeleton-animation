#pragma once
#include "../EnumHeader.h"
#include "IBlendNode.h"

struct BoneLocal;
class Animation;
class Blender
{
    private:
        std::vector<std::pair<BlendNode, IBlendNode*>> _blendTree;
    public:
        Blender(){};
        ~Blender(){};
        void initialize(void);
        IBlendNode* getBlendNode(BlendNode nodeNum);
        void animationUpdate(const std::chrono::steady_clock::time_point& curTime, std::vector<BoneLocal>& boneLocalVector);
        void eraseAnimationCall(const std::chrono::steady_clock::time_point& curTime);
};
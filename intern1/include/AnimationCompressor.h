#pragma once

#include "include/Common.h"
#include <vector>
#include <utility>

class Animation;
struct AnimationData;
class AnimationCompressor
{
    private:
        void getCompressKeyFrame(std::vector<uint32>& frameList);
        std::pair<float, int32> findFramePoint(std::vector<uint32> frameList, uint32 index);
        void dataSwap(AnimationData* node, std::vector<uint32>& frameList);
    
    public:
        AnimationCompressor(){}
        ~AnimationCompressor(){}
        void CompressData(Animation* animation, float errorRange);
    
    private:
        float _errorRange;
        std::vector<std::pair<uint32,glm::quat>>* _KeyFrameData;
};
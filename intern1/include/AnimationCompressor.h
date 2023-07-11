#pragma once

#include "include/Common.h"
#include <vector>
#include <utility>
class Animation;

class AnimationCompressor
{
    private:
        std::vector<uint32> getCompressKeyFrame();
        std::pair<float, int32> findFramePoint(std::vector<uint32> frameList, uint32 index);
    public:
        AnimationCompressor(){}
        ~AnimationCompressor(){}
        std::vector<uint32> getCompressedData(Animation* animation, float errorRange);      
    private:
        float _errorRange;
        std::vector<glm::quat> _KeyFrameData;
};
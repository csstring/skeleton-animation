#pragma once

#include "include/Common.h"
#include <vector>
#include <utility>
class Animation;
typedef std::vector<std::vector<std::pair<uint32, glm::mat4>>> compressedData;

class AnimationCompressor
{
    private:
        std::vector<uint32> getCompressKeyFrame();
        std::pair<uint32, int32> findFramePoint(std::vector<uint32> frameList, uint32 index);
    public:
        AnimationCompressor(){}
        ~AnimationCompressor(){}
        compressedData getCompressedData(Animation* animation, uint32 errorRange);      
    private:
        uint32 _errorRange;
        std::vector<glm::quat> _KeyFrameData;
};
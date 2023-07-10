#include "include/AnimationCompressor.h"
#include "include/Animation.h"
#include "include/GLM/gtx/spline.hpp"
#include <algorithm>
#include "include/GLM/ext.hpp"

std::pair<uint32, int32> AnimationCompressor::findFramePoint(std::vector<uint32> frameList, uint32 index)
{
    uint32 k1 = frameList[index];
    uint32 k2 = frameList[index + 1]; 
    uint32 k3 = frameList[index + 2]; 
    uint32 k4 = frameList[index + 3];
    std::pair<uint32, int32> returnVal({0,-1});

    for (int i = k2 + 1; i < k3; ++i)
    {
        glm::quat point = glm::catmullRom(_KeyFrameData[k1],_KeyFrameData[k1],_KeyFrameData[k1],_KeyFrameData[k1], //선형보간(k2,k3,i));

        /*
        point error < range continue;
        returnval < val
        swap
        */
    }

    return returnVal;
}

std::vector<uint32> AnimationCompressor::getCompressKeyFrame()//뭘 기준으로 압축 할건지
{
    std::vector<uint32> frameList;
    bool flag = true;

    frameList.reserve(_KeyFrameData.size());
    frameList.push_back(0);
    frameList.push_back(1);
    frameList.push_back(_KeyFrameData.size()-1);
    frameList.push_back(_KeyFrameData.size()-2);

    while(flag == true)
    {
        flag = false;
        uint32 maxError = 0;
        uint32 keyFrame;
        for (uint32 i = 0; i < frameList.size()-3; ++i)
        {
            std::pair<uint32, int32> curError = findFramePoint(frameList, i);
            if (curError.second == -1)
                continue;
            flag = true;
            if (curError.first > maxError)
            {
                maxError = curError.first;
                keyFrame = curError.second;
            }
        }
        if (flag == true)
        {
            auto it = std::upper_bound(frameList.begin(), frameList.end(), keyFrame);
            if (it == frameList.end())
                frameList.push_back(keyFrame);
            else
                frameList.insert(it, keyFrame);
        }
    }

    frameList.shrink_to_fit();
    return frameList;
}

compressedData AnimationCompressor::getCompressedData(Animation* animation, uint32 errorRange)
{
    _errorRange = errorRange;
    _KeyFrameData = animation->_rootNode._localRotation;
}
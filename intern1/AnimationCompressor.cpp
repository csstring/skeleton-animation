#include "include/AnimationCompressor.h"
#include "include/Animation.h"
#include "include/GLM/gtx/spline.hpp"
#include <algorithm>
#include "include/GLM/ext.hpp"

std::pair<float, int32> AnimationCompressor::findFramePoint(std::vector<uint32> frameList, uint32 index)
{
    float k1 = frameList[index];
    float k2 = frameList[index + 1]; 
    float k3 = frameList[index + 2]; 
    float k4 = frameList[index + 3];
    std::pair<float, int32> returnVal({0,-1});

    for (float i = k2 + 1; i < k3; ++i)
    {
        glm::quat point = glm::catmullRom(_KeyFrameData[k1],_KeyFrameData[k2],_KeyFrameData[k3],_KeyFrameData[k4], (i-k2) / (k3 - k2));
        float x = _KeyFrameData[i].x - point.x;
        float y = _KeyFrameData[i].y - point.y;
        float z = _KeyFrameData[i].z - point.z;
        float w = _KeyFrameData[i].w - point.w;
        float error = x*x + y*y + z*z + w*w;
        if (error < _errorRange || error < returnVal.first)
            continue;
        returnVal.first = error;
        returnVal.second = i;
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
    frameList.push_back(_KeyFrameData.size()-2);
    frameList.push_back(_KeyFrameData.size()-1);

    while(flag == true)
    {
        flag = false;
        float maxError = 0;
        uint32 keyFrame;
        for (uint32 i = 0; i < frameList.size()-3; ++i)
        {
            std::pair<float, int32> curError = findFramePoint(frameList, i);
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

std::vector<uint32> AnimationCompressor::getCompressedData(Animation* animation, float errorRange)
{
    _errorRange = errorRange;
    _KeyFrameData = animation->_rootNode._localRotation;
    return getCompressKeyFrame();
}
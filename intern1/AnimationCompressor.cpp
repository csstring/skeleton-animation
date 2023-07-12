#include "include/AnimationCompressor.h"
#include "include/Animation.h"
#include "include/GLM/gtx/spline.hpp"
#include <queue>
#include <algorithm>
#include "include/GLM/ext.hpp"

std::pair<float, int32> AnimationCompressor::findFramePoint(std::vector<uint32> frameList, uint32 index)
{
    uint32 k1 = frameList[index];
    uint32 k2 = frameList[index + 1]; 
    uint32 k3 = frameList[index + 2]; 
    uint32 k4 = frameList[index + 3];
    std::pair<float, int32> returnVal({0,-1});
    auto it = _KeyFrameData->begin();

    for (float i = k2 + 1; i < k3; ++i)
    {
        glm::quat point = glm::catmullRom((it + k1)->second,(it + k2)->second,(it + k3)->second,(it + k4)->second, (i-k2) / (k3 - k2));
        float x = (it + i)->second.x - point.x;
        float y = (it + i)->second.y - point.y;
        float z = (it + i)->second.z - point.z;
        float w = (it + i)->second.w - point.w;
        float error = x*x + y*y + z*z + w*w;
        if (error < _errorRange || error < returnVal.first)
            continue;
        returnVal.first = error;
        returnVal.second = i;
    }

    return returnVal;
}

void AnimationCompressor::getCompressKeyFrame(std::vector<uint32>& frameList)
{
    bool flag = true;

    frameList.reserve(_KeyFrameData->size());
    frameList.push_back(0);
    frameList.push_back(1);
    frameList.push_back(_KeyFrameData->size()-2);
    frameList.push_back(_KeyFrameData->size()-1);

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
}
//data swap
void AnimationCompressor::dataSwap(AnimationData* node, std::vector<uint32>& frameList)
{
    std::vector<std::pair<uint32,glm::quat>> R;
    std::vector<std::pair<uint32,glm::mat4>> T;

    R.reserve(frameList.size());
    T.reserve(frameList.size());
    for (uint32 i : frameList)
    {
        R.push_back(node->_localRotation[i]);
        T.push_back(node->_localTrans[i]);
    }
    node->_localRotation.swap(R);
    node->_localTrans.swap(T);
}

void AnimationCompressor::CompressData(Animation* animation, float errorRange)
{
    std::vector<uint32> frameList;
    std::queue<AnimationData*> dataQ;

    frameList.reserve(animation->_rootNode._localRotation.size());
    _errorRange = errorRange;
    dataQ.push(&animation->_rootNode);
    while (dataQ.empty() == false)
    {
        AnimationData* cur = dataQ.front();
        dataQ.pop();

        for (AnimationData& child : cur->_childrens)
            dataQ.push(&child);

        _KeyFrameData = &cur->_localRotation;
        getCompressKeyFrame(frameList);
        dataSwap(cur, frameList);
        frameList.clear();
    }
}
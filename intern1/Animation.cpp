#include "include/Animation.h"
#include "include/AnimationTreeTraversal.h"
#include <queue>

void Animation::getDataNode(const uint32 boneIndex, AnimationData& node, AnimationData** returnNode) const
{
    if (node._boneIndex == boneIndex)
    {
        *returnNode = &node;
        return;
    }
    for (AnimationData& childNode : node._childrens)
    {
        getDataNode(boneIndex, childNode, returnNode);
    }
    return;
}

AnimationData* Animation::returnAnimationData(const uint32 boneIndex)
{   
    AnimationData* returnNode = NULL;
    getDataNode(boneIndex, _rootNode, &returnNode);

    return returnNode;
}

void Animation::getDataNode(const uint32 boneIndex, const AnimationData& node, const AnimationData** returnNode) const
{
    if (node._boneIndex == boneIndex)
    {
        *returnNode = &node;
        return;
    }
    for (const AnimationData& childNode : node._childrens)
    {
        getDataNode(boneIndex, childNode, returnNode);
    }
    return;
}


const AnimationData* Animation::returnAnimationData(const uint32 boneIndex) const
{   
    const AnimationData* returnNode = NULL;
    getDataNode(boneIndex, _rootNode, &returnNode);

    return returnNode;
}

void Animation::AnimationDataTraver(AnimationTreeTraversal& travel)
{
    std::queue<AnimationData* > dataQueue;

    dataQueue.push(&_rootNode);
    while (dataQueue.size() != 0)
    {
        AnimationData* curData = dataQueue.front();
        dataQueue.pop();

        travel.processNode(*curData);
        for (int i =0; i < curData->_childrens.size(); ++i)
            dataQueue.push(&curData->_childrens[i]);
    }
}

void Animation::findSameFrame(uint32 count)
{
    std::queue<AnimationData* > dataQueue;
    std::vector<glm::vec3> origin[31];
    std::vector<uint32> frameList;
    float errorRange = 0;
    int curCount = 0;

    dataQueue.push(&_rootNode);
    while (dataQueue.size() != 0)
    {
        AnimationData* curData = dataQueue.front();
        dataQueue.pop();
        for (auto& mt : curData->_localTrans)
        {
            origin[curData->_boneIndex].push_back(mt.second);
        }
        for (int i =0; i < curData->_childrens.size(); ++i)
        dataQueue.push(&curData->_childrens[i]);
    }
    // errorRange += glm::abs(origin[0][0].x - origin[1][i].x);
    // errorRange += glm::abs(origin[0][0].y - origin[1][i].y);
    // errorRange += glm::abs(origin[0][0].z - origin[1][i].z);
    while (curCount < count)
    {
        frameList.clear();
        curCount = 0;
        for (int i = 1; i < origin[0].size(); ++i)
        {
            float curError = 0;
            for (int j = 0; j < 31; ++j)
            {
                curError += glm::abs(origin[j][0].x - origin[j][i].x);
                curError += glm::abs(origin[j][0].y - origin[j][i].y);
                curError += glm::abs(origin[j][0].z - origin[j][i].z);
            }
            if (errorRange == 0)
                errorRange = curError;
            if (curError < errorRange)
            {
                frameList.push_back(i);
                curCount++;
            }
        }
        errorRange *= 1.02;
    }
    for (auto it : frameList)
    {
        std::cout << "frame : " << it << std::endl;
    }
}
#include "include/Animation.h"
#include "include/AnimationTreeTraversal.h"
#include <queue>

void getDataNode(const uint32 boneIndex, AnimationData& node, AnimationData** returnNode)
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

void Animation::AnimationDataTraver(AnimationTreeTraversal& travel)
{
    std::queue<AnimationData* > dataQueue;

    dataQueue.push(&_rootNode);
    while (dataQueue.size() != 0)
    {
        AnimationData* curData = dataQueue.front();
        dataQueue.pop();

        travel.processNode(*curData);//fixme
        for (int i =0; i < curData->_childrens.size(); ++i)
            dataQueue.push(&curData->_childrens[i]);
    }
}
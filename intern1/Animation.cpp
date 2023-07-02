#include "include/Animation.h"
#include "AnimationTreeTraversal.h"
#include "include/GL/glew.h"
#include <queue>

void getDataNode(const std::string& name, AnimationData& node, AnimationData** returnNode)
{
    if (node._name == name)
    {
        *returnNode = &node;
        return;
    }
    for (AnimationData& childNode : node._childrens)
    {
        getDataNode(name, childNode, returnNode);
    }
    return;
}

AnimationData* Animation::returnAnimationData(const std::string& name)
{   
    AnimationData* returnNode = NULL;
    getDataNode(name, _rootNode, &returnNode);

    return returnNode;
}

void Animation::AnimationDataTraver(AnimationTreeTraversal& travel)
{
    std::queue<AnimationData&> dataQueue;

    dataQueue.push(_rootNode);
    while (dataQueue.size() != 0)
    {
        AnimationData& curData = dataQueue.front();
        dataQueue.pop();

        travel.processNode(curData);
        for (AnimationData& child : curData._childrens)
            dataQueue.push(child);
    }
}
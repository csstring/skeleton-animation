#include "Animation.h"



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
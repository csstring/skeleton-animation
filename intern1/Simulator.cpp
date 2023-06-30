#include "Simulator.h"
#include "Animation.h"
#include "Skeleton.h"

void Simulator::setPoseDfs(std::vector<Bone>& boneVector, const AnimationData& node)
{
    if (node._name != "root")
    {
        Bone& child = boneVector[node._boneIndex];
        Bone& parent = boneVector[node._parentIndex];

        child._postion.x = parent._postion.x + child._direction.x * child._length *_skeleton->getGBL();
        child._postion.y = parent._postion.y + child._direction.y * child._length *_skeleton->getGBL();
        child._postion.z = parent._postion.z + child._direction.z * child._length *_skeleton->getGBL();
        child.setupBone();
    }
    for (const AnimationData& data : node._childrens)
        setPoseDfs(boneVector, data);
}

void Simulator::setInitPose(void)
{
    setPoseDfs(_skeleton->getBoneVector(), _animation->_rootNode);
}
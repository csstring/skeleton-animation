#include "include/Skeleton.h"
#include "include/Bone.h"

void Skeleton::draw(void) const 
{
    for (const Bone& bone : _boneVector)
    {
        bone.draw();
    }
}

int32 Skeleton::findBoneIndex(const std::string& name) const
{
    for (int i =0; i < _boneVector.size(); ++i)
    {
        if (_boneVector[i]._boneName == name)
            return i;
    }
    return -1;
}
#include "include/Skeleton.h"
#include "include/GLM/gtx/quaternion.hpp"
/*
glm::quat localRot = bone._c * glm::quat_cast(matrix) * bone._invC;
animationData->_localRotation.push_back({animationTime, localRot});
glm::vec3 transV = glm::toMat3(localRot) * bone._b + localTransV;
*/
int32 Skeleton::findBoneIndex(const std::string& name) const
{
    for (int i =0; i < _boneVector.size(); ++i)
    {
        if (_boneVector[i]._boneName == name)
            return i;
    }
    return -1;
}

glm::vec3 Skeleton::getCharLocalPosition(BONEID boneID)
{
    glm::mat4 matrix(1.0f);
    const Bone* bone = &_boneVector[boneID];
    while (true)
    {   
        glm::quat localRot = bone->_c * glm::quat_cast(matrix) * bone->_invC;
        glm::vec3 transV = glm::toMat3(localRot) * bone->_b;
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), transV);

        matrix = trans * glm::toMat4(localRot) * matrix;
        if (bone->_parentBoneIndex == -1)
            break;
        bone = &_boneVector[bone->_parentBoneIndex];
    }
    return matrix * glm::vec4(0,0,0,1);
}

float Skeleton::getSkeletonHeight(void)
{
    glm::vec3 headPos = getCharLocalPosition(BONEID::HEAD);
    glm::vec3 rfootPos = getCharLocalPosition(BONEID::RFOOT);

    return (headPos.y - rfootPos.y);
}

float Skeleton::getSkeletonWidth(void)
{
    glm::vec3 rightHandPos = getCharLocalPosition(BONEID::RCLAVICLE);
    glm::vec3 leftHandPos = getCharLocalPosition(BONEID::LCLAVICLE);

    return glm::length(rightHandPos - leftHandPos);
}
#include "../include/AnimationBlend/IBlendNode.h"
#include "../include/BoneLocal.h"
#include "../include/GLM/gtx/spline.hpp"
#include <queue>

inline bool pairCompare(const std::pair<uint32, glm::quat>& a, const uint32& val)
{
    return a.first < val;
}

void IBlendNode::getTransFormByKeyFrame(glm::quat& interpolR, glm::vec3& interpolT, const AnimationData* node, uint32 keyFrame)
{
    auto itR = std::lower_bound(node->_localRotation.begin(), node->_localRotation.end(), keyFrame, pairCompare);
    if (itR == node->_localRotation.end()) 
        itR--;
    if (itR + 1 != node->_localRotation.end())
        itR++;
    uint32 frontDistance = std::distance(node->_localRotation.begin(), itR);
    if (frontDistance < 3) itR = node->_localRotation.begin() + 3;
    if (keyFrame <= (itR -2)->first) keyFrame = (itR -2)->first + 1;
    else if (keyFrame > (itR - 1)->first) keyFrame = (itR - 1)->first;
    const glm::quat& t0 = (itR - 3)->second;
    const glm::quat& t1 = (itR - 2)->second;
    const glm::quat& t2 = (itR - 1)->second;
    const glm::quat& t3 = (itR - 0)->second;
    interpolR = glm::catmullRom(t0,t1,t2,t3, (keyFrame - (itR -2)->first)/((itR-1)->first - (itR -2)->first));

    auto itT = std::lower_bound(node->_localTrans.begin(), node->_localTrans.end(), keyFrame, pairCompare);
    itT++;
    const glm::vec3& v0 = (itT - 3)->second;
    const glm::vec3& v1 = (itT - 2)->second;
    const glm::vec3& v2 = (itT - 1)->second;
    const glm::vec3& v3 = (itT - 0)->second;
    interpolT = glm::catmullRom(v0,v1,v2,v3,(keyFrame - (itT -2)->first)/((itT-1)->first - (itT -2)->first));
}

void IBlendNode::updateTransForm(const AnimationData& node, uint32 keyFrame, float interpolVal, std::vector<BoneLocal>& _boneLocalVector)
{
    std::queue<const AnimationData*> animationQ;
    
    if (interpolVal > 1)
        interpolVal = 1;

    animationQ.push(&node);
    while (animationQ.empty() == false)
    {
        const AnimationData* curData = animationQ.front();
        animationQ.pop();
        for (const AnimationData& animation : curData->_childrens)
            animationQ.push(&animation);
        glm::quat mixR;
        glm::vec3 mixT;
        getTransFormByKeyFrame(mixR, mixT, curData, keyFrame);

        _boneLocalVector[curData->_boneIndex].translationInBoneLocal = glm::mix(_boneLocalVector[curData->_boneIndex].translationInBoneLocal, mixT, interpolVal);
        _boneLocalVector[curData->_boneIndex].rotationInBoneLocal = glm::slerp(_boneLocalVector[curData->_boneIndex].rotationInBoneLocal, mixR, interpolVal);
    }
}

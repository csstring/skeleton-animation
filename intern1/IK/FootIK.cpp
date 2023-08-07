#include "../include/IK/FootIk.h"
#include "../include/GLM/gtx/quaternion.hpp"
#include "../include/GLM/gtc/quaternion.hpp"
#include "../include/Controller.h"
#include "../include/Character.h"
#include "../include/Bone.h"
#include "../include/IK/IKUtility.h"
//ground찾는건 월드 에서 하고 찾았으면 캐릭터 로컬에서 계산
//0 rhipjoint 1 rfemur 2 rtibia 3 rfoot
void FootIK::findGround(void)
{

}

void FootIK::setGroundNormal(glm::vec3 normal)
{
    _groundNormal = normal;
}

bool FootIK::reachable(const std::vector<glm::vec3>& inCharacterPos, std::vector<float>& distance, glm::vec3 footPosition)
{
    glm::vec3 start = inCharacterPos.front();
    glm::vec3 end = inCharacterPos.back();
    float targetDistance = glm::length(footPosition - start);
    float curDistance = 0;
    for (uint32 i = 1; i < inCharacterPos.size(); ++i)
    {
        end = inCharacterPos[i];
        distance.push_back(glm::length(start - end));
        start = end;
    }
    curDistance = distance[0] + distance[1];
    if (curDistance < targetDistance)
        return false;
    return true;
}

void FootIK::positionFixLimitAngleBackWard(glm::vec3& start, glm::vec3& end, glm::vec3 endBoneDir,const Bone& endBone)
{
    glm::vec3 initialDirection = glm::normalize(end - start);
    glm::vec3 targetDirection = glm::normalize(endBoneDir);
    glm::quat rotation = glm::rotation(initialDirection, targetDirection);
    glm::vec3 eulerAngle = glm::eulerAngles(rotation);

    for (auto& limit : endBone._limits)
    {
        DOF dof;
        float min, max;
        std::tie(dof, min, max) = limit;
        if (dof == DOF::RX)
            eulerAngle.x = glm::clamp(eulerAngle.x, min, max);
        else if (dof == DOF::RY)
            eulerAngle.y = glm::clamp(eulerAngle.y, min, max);
        else if (dof == DOF::RZ)
            eulerAngle.z = glm::clamp(eulerAngle.z, min, max);
    }
    rotation = glm::quat(eulerAngle);

    glm::vec3 correctPos = glm::inverse(rotation) * targetDirection * glm::length(end - start);

    start = end - correctPos;

}

void FootIK::positionFixLimitAngleForWard(glm::vec3& start, glm::vec3& end, glm::vec3 startBoneDir,const Bone& endBone)
{
    glm::vec3 initialDirection = glm::normalize(startBoneDir);
    glm::vec3 targetDirection = glm::normalize(end - start);
    glm::quat rotation = glm::rotation(initialDirection, targetDirection);
    glm::vec3 eulerAngle = glm::eulerAngles(rotation);

    for (auto& limit : endBone._limits)
    {
        DOF dof;
        float min, max;
        std::tie(dof, min, max) = limit;
        if (dof == DOF::RX)
            eulerAngle.x = glm::clamp(eulerAngle.x, min, max);
        else if (dof == DOF::RY)
            eulerAngle.y = glm::clamp(eulerAngle.y, min, max);
        else if (dof == DOF::RZ)
            eulerAngle.z = glm::clamp(eulerAngle.z, min, max);
    }
    rotation = glm::quat(eulerAngle);

    glm::vec3 correctPos = rotation * initialDirection * glm::length(end - start);
    end = start + correctPos;
}

void FootIK::solveIK(
    std::vector<BoneLocal>& _boneLocalVector, 
    const glm::mat4& worldRotation, 
    const glm::mat4& worldTranslate, 
    const Controller& controller,
    const std::chrono::steady_clock::time_point& curTime)
{
    this->_blendingRatio = 1;//default

    std::vector<glm::vec3> inCharLocalPos;
    std::vector<glm::mat4> inCharLocalRot;
    std::vector<glm::mat4> inCharTrans;
    std::vector<float> distance;

    for (uint32 i : _boneIndexVec)
    {
        inCharTrans.push_back(controller.getMatrixInCharLocal(i, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector));
        inCharLocalPos.push_back(inCharTrans.back() * glm::vec4(0,0,0,1));
        inCharLocalRot.push_back(glm::mat3(inCharTrans.back()));
    }

    glm::mat4 charLocalToWorld = worldTranslate * worldRotation;
    glm::vec3 targetPosInCharLocal = glm::inverse(charLocalToWorld) * glm::vec4(_targetPosition, 1);
    targetPosInCharLocal.x = inCharLocalPos[3].x;
    targetPosInCharLocal.z = inCharLocalPos[3].z;
    ////////////////////////////////////////////
    _groundNormal = glm::inverse(charLocalToWorld) * glm::vec4(_groundNormal,0);
    glm::vec3 parentDir = glm::normalize(inCharLocalPos[2] - inCharLocalPos[3]);
    glm::vec3 axis = glm::normalize(glm::cross(_groundNormal, parentDir));
    glm::vec3 groundDir = glm::normalize(glm::cross(axis, _groundNormal));
    glm::vec3 Pos3InChar = targetPosInCharLocal;//tmp

    float distance2 = glm::length(inCharLocalPos[2] - inCharLocalPos[3]);
    glm::vec3 footPosInChar = Pos3InChar + groundDir * distance2;

    if (reachable(inCharLocalPos, distance, footPosInChar) == false)//distance 구하는걸로 수정, fix me
        return;

    glm::vec3 start = inCharLocalPos.front();
    uint32 iterCount = 0;
    while (glm::length(footPosInChar - inCharLocalPos[2]) > 0.1)
    {
        iterCount++;
        if (iterCount >= 30)
        {
            std::cout << "Fook ik length val : " << glm::length(footPosInChar - inCharLocalPos[2]) << std::endl;
            return;
        }
            
        inCharLocalPos[2] = footPosInChar;
        glm::vec3 BoneDir;
        for(uint16 i = _boneIndexVec.size()-2; i >=1; --i)
        {
            float r = glm::length(inCharLocalPos[i] - inCharLocalPos[i-1]);
            float k = distance[i-1] / r;
            inCharLocalPos[i-1] = glm::mix(inCharLocalPos[i], inCharLocalPos[i-1], k);
            if (i == _boneIndexVec.size()-1)
                BoneDir = Pos3InChar - inCharLocalPos[i];
            else
                BoneDir = inCharLocalPos[i+1] - inCharLocalPos[i];
            positionFixLimitAngleBackWard(inCharLocalPos[i-1], inCharLocalPos[i], BoneDir ,_boneVector[_boneIndexVec[i+1]]);
        }

        inCharLocalPos.front() = start;
        for(uint16 i = 0; i < _boneIndexVec.size()-2; ++i)
        {
            float r = glm::length(inCharLocalPos[i] - inCharLocalPos[i+1]);
            float k = distance[i] / r;
            inCharLocalPos[i+1] = glm::mix(inCharLocalPos[i], inCharLocalPos[i+1], k);
            if (i == 0)
                BoneDir = inCharLocalRot[i] * glm::vec4(_boneVector[_boneIndexVec[0]]._direction,0);
            else
                BoneDir = inCharLocalPos[i] - inCharLocalPos[i-1];
            positionFixLimitAngleForWard(inCharLocalPos[i], inCharLocalPos[i+1], BoneDir, _boneVector[_boneIndexVec[i+1]]);
        }
    }

    inCharLocalPos[3] = targetPosInCharLocal;
    glm::vec3 inCharDir3 = glm::normalize(inCharLocalPos[3] - inCharLocalPos[2]);
    glm::vec3 inCharDir2 = glm::normalize(inCharLocalPos[2] - inCharLocalPos[1]);
    glm::vec3 inCharDir1 = glm::normalize(inCharLocalPos[1] - inCharLocalPos[0]);
    inCharLocalRot[3] = glm::toMat4(glm::rotation(_boneVector[_boneIndexVec[3]]._direction ,inCharDir3));
    inCharLocalRot[2] = glm::toMat4(glm::rotation(_boneVector[_boneIndexVec[2]]._direction ,inCharDir2));
    inCharLocalRot[1] = glm::toMat4(glm::rotation(_boneVector[_boneIndexVec[1]]._direction ,inCharDir1));

    glm::mat4 trans3, trans2, trans1, trans0;
    trans3 = glm::translate(glm::mat4(1.0f), inCharLocalPos[3]) * inCharLocalRot[3];
    trans2 = glm::translate(glm::mat4(1.0f), inCharLocalPos[2]) * inCharLocalRot[2];
    trans1 = glm::translate(glm::mat4(1.0f), inCharLocalPos[1]) * inCharLocalRot[1];
    trans0 = glm::translate(glm::mat4(1.0f), inCharLocalPos[0]) * inCharLocalRot[0];

    glm::vec3 bonePos3, bonePos2, bonePos1;
    glm::quat boneRot3, boneRot2, boneRot1;

    bonePos3 = glm::inverse(trans2) * glm::vec4(inCharLocalPos[3], 1);
    bonePos2 = glm::inverse(trans1) * glm::vec4(inCharLocalPos[2], 1);
    bonePos1 = glm::inverse(trans0) * glm::vec4(inCharLocalPos[1], 1);

    boneRot3 = glm::quat(glm::inverse(trans2) * inCharLocalRot[3]);
    boneRot2 = glm::quat(glm::inverse(trans1) * inCharLocalRot[2]);
    boneRot1 = glm::quat(glm::inverse(trans0) * inCharLocalRot[1]);

    _boneLocalVector[_boneIndexVec[3]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[3]].translationInBoneLocal,bonePos3, _blendingRatio);
    _boneLocalVector[_boneIndexVec[2]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[2]].translationInBoneLocal,bonePos2, _blendingRatio);
    _boneLocalVector[_boneIndexVec[1]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[1]].translationInBoneLocal,bonePos1, _blendingRatio);

    _boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal, boneRot3, _blendingRatio);
    _boneLocalVector[_boneIndexVec[2]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[2]].rotationInBoneLocal, boneRot2, _blendingRatio);
    _boneLocalVector[_boneIndexVec[1]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[1]].rotationInBoneLocal, boneRot1, _blendingRatio);
    
}

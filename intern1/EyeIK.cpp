#include "include/EyeIK.h"
#include <algorithm>
#include "include/GLM/gtx/quaternion.hpp"
#include "include/GLM/gtc/quaternion.hpp"
#include "include/EnumHeader.h"
#include "include/Controller.h"
#include "include/Character.h"
#include "include/Bone.h"
// #include "include/Skeleton.h"
//parent bone 파싱 넣어야함
void EyeIK::setTargetPosition(glm::vec3 targetPosition)
{
    _targetPosition = targetPosition;
}

void EyeIK::initialize(void)
{
    const Bone* curBone = nullptr;
    for (const Bone& bone : _boneVector)
    {
        if (bone._boneIndex == BONEID::HEAD)
        {
            curBone = &bone;
            break;
        }
    }
    while (true)
    {
        _eyeBoneIndex.push_back(curBone->_boneIndex);
        _bonedirection.push_back(curBone->_direction);
        curBone = &_boneVector[curBone->_parentBoneIndex];
        if (curBone->_boneIndex == BONEID::UPPERBACK)
            break;
    }
    std::reverse(_eyeBoneIndex.begin(), _eyeBoneIndex.end());
    _IKTranspos.resize(_boneVector.size(), glm::mat4(0.0f));
    _targetPosition = glm::vec3(10000,10000,10000);
}

bool EyeIK::targetPositionCheck(const std::vector<glm::mat4>& characterTranspos)
{
    //최대 회전 각도 생각해서 볼수 있는지 체크
    if (_targetOn == false)
    {
        _start = getCurTimePoint();
        _targetOn = true;
        return true;
    }
    else 
        return true;
}

bool EyeIK::reachable(const std::vector<glm::mat4>& characterTranspos, std::vector<float>& distance, glm::vec3 headPosition)
{
    glm::vec3 start = characterTranspos[_eyeBoneIndex.front()] * glm::vec4(0,0,0,1);
    glm::vec3 end = characterTranspos[_eyeBoneIndex.back()] * glm::vec4(0,0,0,1);
    float targetDistance = glm::length(headPosition - start);
    float curDistance = 0;
    for (uint32 i = 1; i < _eyeBoneIndex.size(); ++i)
    {
        end = characterTranspos[_eyeBoneIndex[i]] * glm::vec4(0,0,0,1);
        distance.push_back(glm::length(start - end));
        curDistance += distance.back();
        start = end;
    }
    return true;
}

bool EyeIK::limitAngleCheck(const Bone& bone, const glm::quat& boneRot)
{
    glm::vec3 eulerAngle = glm::eulerAngles(boneRot);

    for (auto& limit : bone._limits)
    {
        DOF dof;
        float min, max;
        std::tie(dof, min, max) = limit;
        if (dof == DOF::RX && (eulerAngle.x < min || max < eulerAngle.x))
            return false;
        else if (dof == DOF::RY && (eulerAngle.y < min || max < eulerAngle.y))
            return false;
        else if (dof == DOF::RZ && (eulerAngle.z < min || max < eulerAngle.z))
            return false;
    }
    return true;
}

glm::vec3 EyeIK::moveInBoneLocalPos(const glm::vec3& start, const glm::vec3& end, const glm::quat& toTargetDir, const glm::vec3& endBoneDir, float ratio)//비율
{
    float distance = glm::length(end - start);
    glm::vec3 initialPos = start + endBoneDir * distance;
    // glm::vec3 targetDir = quatToEulerDivideRatio(toTargetDir, ratio);
    glm::vec3 targetPos = start + quatDivideRatio(toTargetDir, ratio) * endBoneDir * distance;
    
    return (targetPos - initialPos); 
}
/*
quat -> 오일러 -> 제한 각도 확인하고 비율로 오일러 회전각 정해주고 -> quat -> rot
ratio, 
*/
void EyeIK::solveEyeIK(std::vector<BoneLocal>& _boneLocalVector, const glm::mat4& worldRotation, const glm::mat4& worldTranslate, const Controller& controller)
{
    std::vector<glm::vec3> inCharLocalPos;
    std::vector<glm::mat4> inCharLocalRot;
    //const auto& boneVector = controller.getPlayer()->getCharacterSkeleton().getBoneVector();
    // glm::quat headBoneLocal = glm::inverse(controller.getMatrixInCharLocal(_eyeBoneIndex.back(), controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector));

    for (uint32 i : _eyeBoneIndex)
    {
        glm::mat4 inCharTrans = controller.getMatrixInCharLocal(i, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector);
        inCharLocalPos.push_back(inCharTrans * glm::vec4(0,0,0,1));
        inCharLocalRot.push_back(glm::mat3(inCharTrans));
    }

    glm::mat4 charLocalToWorld = worldTranslate * worldRotation;
    glm::vec3 targetPosInCharLocal = glm::inverse(charLocalToWorld) * glm::vec4(_targetPosition, 1);
    glm::vec3 targetDir = targetPosInCharLocal - inCharLocalPos.back();
    glm::vec3 curSee = inCharLocalRot.back() * glm::vec4(glm::cross(glm::vec3(1,0,0), _bonedirection.back()),1);
    glm::quat afterSee = glm::rotation(glm::normalize(curSee), glm::normalize(targetDir));

    // glm::vec3 headTopPos = inCharLocalRot.back() * glm::vec4(_bonedirection.back(),1);
    // glm::vec3 initialDir = inCharLocalRot[1] * glm::vec4(_bonedirection.back(), 1); 
    // glm::vec3 initialSee = glm::cross(glm::vec3(1,0,0), initialDir);
    // glm::vec3 toTargetDir = afterSee * headTopPos;
    // glm::quat afterSeetmp = glm::rotation(glm::normalize(initialSee), glm::normalize(targetDir));
    // glm::vec3 toTargetTmp = afterSeetmp * _bonedirection.back();
    for (uint32 i : _eyeBoneIndex)
    {
        glm::quat rot;
        if (i == BONEID::LOWERNECK)
        {
            rot = quatDivideRatio(afterSee,LOWERNECK_RATIO) * _boneLocalVector[i].rotationInBoneLocal;
            if (limitAngleCheck(_boneVector[i], rot) == false)
            {
                glm::vec3 deg = quatToEulerDivideRatio(rot, 1) * (180.0f / PI);
                std::cout << glm::to_string(deg) << std::endl;
                return;
            }
        }
        else if (i == BONEID::UPPERNECK)
        {
            rot = quatDivideRatio(afterSee,UPPER_RATIO) * _boneLocalVector[i].rotationInBoneLocal;
            if (limitAngleCheck(_boneVector[i], rot) == false)
            {
                glm::vec3 deg = quatToEulerDivideRatio(rot, 1) * (180.0f / PI);
                std::cout << glm::to_string(deg) << std::endl;
                return;
            }
        }
        else if (i == BONEID::HEAD)
        {
            rot = quatDivideRatio(afterSee,HEAD_RATIO) * _boneLocalVector[i].rotationInBoneLocal;
            if (limitAngleCheck(_boneVector[i], rot) == false)
            {
                glm::vec3 deg = quatToEulerDivideRatio(rot, 1) * (180.0f / PI);
                std::cout << glm::to_string(deg) << std::endl;
                return;
            }
        }
    }
    for (uint32 i = 0; i < _eyeBoneIndex.size(); ++i)
    {
        if (_eyeBoneIndex[i] == BONEID::LOWERNECK)
        {
            _boneLocalVector[_eyeBoneIndex[i]].rotationInBoneLocal = quatDivideRatio(afterSee,LOWERNECK_RATIO) * _boneLocalVector[_eyeBoneIndex[i]].rotationInBoneLocal;
            _boneLocalVector[_eyeBoneIndex[i]].translationInBoneLocal += moveInBoneLocalPos(inCharLocalPos[i-1], inCharLocalPos[i], afterSee, _bonedirection[i], LOWERNECK_RATIO);
        }
        else if (_eyeBoneIndex[i] == BONEID::UPPERNECK)
        {
            _boneLocalVector[_eyeBoneIndex[i]].rotationInBoneLocal = quatDivideRatio(afterSee,UPPER_RATIO) * _boneLocalVector[_eyeBoneIndex[i]].rotationInBoneLocal;
            _boneLocalVector[_eyeBoneIndex[i]].translationInBoneLocal += moveInBoneLocalPos(inCharLocalPos[i-1], inCharLocalPos[i], afterSee, _bonedirection[i], UPPER_RATIO);
        }
        else if (_eyeBoneIndex[i] == BONEID::HEAD)
        {
            _boneLocalVector[_eyeBoneIndex[i]].rotationInBoneLocal = quatDivideRatio(afterSee,HEAD_RATIO) * _boneLocalVector[_eyeBoneIndex[i]].rotationInBoneLocal;
            _boneLocalVector[_eyeBoneIndex[i]].translationInBoneLocal += moveInBoneLocalPos(inCharLocalPos[i-1], inCharLocalPos[i], afterSee, _bonedirection[i], HEAD_RATIO);
        }
    }
}
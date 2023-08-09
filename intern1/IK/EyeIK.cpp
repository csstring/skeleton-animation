#include "../include/IK/EyeIK.h"
#include "../include/GLM/gtx/quaternion.hpp"
#include "../include/GLM/gtc/quaternion.hpp"
#include "../include/EnumHeader.h"
#include "../include/Controller.h"
#include "../include/Character.h"
#include "../include/Bone.h"
#include "../include/IK/IKUtility.h"
//init HEAD, UPPERBACK

glm::vec3 EyeIK::moveInBoneLocalPos(const glm::vec3& start, const glm::vec3& end, const glm::quat& toTargetDir, const glm::vec3& endBoneDir, float ratio)//비율
{
    float distance = glm::length(end - start);
    glm::vec3 initialPos = start + endBoneDir * distance;
    glm::vec3 targetPos = start + quatDivideRatio(toTargetDir, ratio) * endBoneDir * distance;
    
    return (targetPos - initialPos); 
}

void EyeIK::blendingRatioUpdate(const std::chrono::steady_clock::time_point& curTime)
{
    std::chrono::milliseconds  millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _callTime);
    if (_targetOn == false && _blendingRatio <= 0)
    {
        _callTime = curTime;
        return;
    }
    else if (_targetOn == false && _blendingRatio > 0)
        _blendingRatio -= static_cast<float>(millisecond.count()) / OVERLAPTIME;
    else if (_targetOn == true && _blendingRatio < 1)
        _blendingRatio += static_cast<float>(millisecond.count()) / OVERLAPTIME;
    if (_blendingRatio > 1) 
        _blendingRatio = 1;
    else if (_blendingRatio < 0) 
        _blendingRatio = 0;
    
    _callTime = curTime;
}

void EyeIK::solveIK(
    std::vector<BoneLocal>& _boneLocalVector, 
    const glm::mat4& worldRotation, 
    const glm::mat4& worldTranslate, 
    const Controller& controller,
    const std::chrono::steady_clock::time_point& curTime,
    physx::PxScene* gScene
)
{
    std::vector<glm::vec3> inCharLocalPos;
    std::vector<glm::mat4> inCharLocalRot;

    for (uint32 i : _boneIndexVec)
    {
        glm::mat4 inCharTrans = controller.getMatrixInCharLocal(i, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector);
        inCharLocalPos.push_back(inCharTrans * glm::vec4(0,0,0,1));
        inCharLocalRot.push_back(glm::mat3(inCharTrans));
    }

    if (_isFirst == true)
    {
        _isFirst = false;
        _callTime = curTime;
    }

    glm::mat4 charLocalToWorld = worldTranslate * worldRotation;
    glm::vec3 targetPosInCharLocal = glm::inverse(charLocalToWorld) * glm::vec4(_targetPosition, 1);
    glm::vec3 middleEye = glm::mix(*(inCharLocalPos.end()-2), *(inCharLocalPos.end()-1),0.8);
    glm::vec3 targetDir = targetPosInCharLocal - middleEye;
    glm::vec3 curSee = inCharLocalRot.back() * glm::vec4(glm::cross(glm::vec3(1,0,0), _bonedirection.back()),1);
    glm::quat afterSee = glm::rotation(glm::normalize(curSee), glm::normalize(targetDir));

    for (uint32 i : _boneIndexVec)
    {
        glm::quat rot;
        if (i == BONEID::LOWERNECK)
            rot = quatDivideRatio(afterSee,LOWERNECK_RATIO) * _boneLocalVector[i].rotationInBoneLocal;
        else if (i == BONEID::UPPERNECK)
            rot = quatDivideRatio(afterSee,UPPER_RATIO) * _boneLocalVector[i].rotationInBoneLocal;
        else if (i == BONEID::HEAD)
            rot = quatDivideRatio(afterSee,HEAD_RATIO) * _boneLocalVector[i].rotationInBoneLocal;
        else if (i == BONEID::THORAX)
            continue;
        if (limitAngleCheck(_boneVector[i], rot) == false)
        {
            // glm::vec3 deg = quatToEulerDivideRatio(rot, 1) * (180.0f / PI);
            // std::cout << glm::to_string(deg) << std::endl;
            _targetOn = false;
            break;
        }
        _targetOn = true;
    }

    blendingRatioUpdate(curTime);

    for (uint32 i = 0; i < _boneIndexVec.size(); ++i)
    {
        float ratio;
        if (_boneIndexVec[i] == BONEID::LOWERNECK)
            ratio = LOWERNECK_RATIO;
        else if (_boneIndexVec[i] == BONEID::UPPERNECK)
            ratio = UPPER_RATIO;
        else if (_boneIndexVec[i] == BONEID::HEAD)
            ratio = HEAD_RATIO;
        else if (_boneIndexVec[i] == BONEID::THORAX)
            continue;
        glm::quat mixRot = quatDivideRatio(afterSee,ratio) * _boneLocalVector[_boneIndexVec[i]].rotationInBoneLocal;
        glm::vec3 mixTranslate = _boneLocalVector[_boneIndexVec[i]].translationInBoneLocal + moveInBoneLocalPos(inCharLocalPos[i-1], inCharLocalPos[i], afterSee, _bonedirection[i], ratio);
        _boneLocalVector[_boneIndexVec[i]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[i]].rotationInBoneLocal, mixRot, _blendingRatio);
        _boneLocalVector[_boneIndexVec[i]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[i]].translationInBoneLocal, mixTranslate, _blendingRatio);
    }
}
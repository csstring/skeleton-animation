#include "include/EyeIK.h"
#include "include/Bone.h"
#include <algorithm>
#include "include/GLM/gtx/quaternion.hpp"
#include "include/GLM/gtc/quaternion.hpp"
#include "include/EnumHeader.h"
#include "include/Controller.h"
#include "include/Character.h"
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
        if (curBone->_boneIndex == BONEID::THORAX)
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

/*
quat -> 오일러 -> 제한 각도 확인하고 비율로 오일러 회전각 정해주고 -> quat -> rot
*/
const std::vector<glm::mat4>& EyeIK::solveEyeIK(
    const std::vector<BoneLocal>& _boneLocalVector, 
    const glm::mat4& worldRotation, 
    const glm::mat4& worldTranslate, 
    const Controller& controller
)
{
    std::vector<glm::vec3> inCharLocalPos;
    std::vector<glm::mat4> inCharLocalRot;
    std::vector<float> distance;

    for (auto& it : _IKTranspos)
        it = glm::mat4(0.0f);
    for (uint32 i : _eyeBoneIndex)
    {
        // glm::mat4 inCharTrans = controller.getMatrixInCharLocal(i, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector);
        // inCharLocalPos.push_back(inCharTrans * glm::vec4(0,0,0,1));
        // inCharLocalRot.push_back(glm::mat3(inCharTrans));
    }
    //rotation
    //cursee를 살짝 아래보게하면 자연스러울듯
    //init head target
    glm::mat4 charLocalToWorld = worldTranslate * worldRotation;
    glm::vec3 targetPosInCharLocal = glm::inverse(charLocalToWorld) * glm::vec4(_targetPosition, 1);

    glm::vec3 targetDir = targetPosInCharLocal - inCharLocalPos.back();
    glm::vec3 headTopPos = inCharLocalRot.back() * glm::vec4(_bonedirection.back(),1);
    glm::vec3 curSee = inCharLocalRot.back() * glm::vec4(glm::cross(glm::vec3(1,0,0), _bonedirection.back()),1);
    glm::quat afterSee = glm::rotation(glm::normalize(curSee), glm::normalize(targetDir));
    glm::vec3 headMove = afterSee * headTopPos;
    inCharLocalRot.back() = glm::toMat4(afterSee) * inCharLocalRot.back();

    //inCharLocalRot[_bonedirection.size()-2] = glm::mix(inCharLocalRot[_bonedirection.size()-1], inCharLocalRot[_bonedirection.size()-3],0.5);
    //if reachable position
    //postion IK
    // if (reachable(characterTranspos, distance, headPosition) == true)//distance 구하는걸로 수정, fix me
    // {
    //     glm::vec3 start = inCharLocalPos.front();
    //     uint32 iterCount = 0;
    //     while (glm::length(headPosition - inCharLocalPos.back()) > 0.1)
    //     {
    //         iterCount++;
    //         if (iterCount >= 10)
    //         {
    //             return _IKTranspos;
    //         }
    //         inCharLocalPos.back() = headPosition;
    //         glm::quat startBonedir;
    //         for(uint16 i = _eyeBoneIndex.size()-1; i >=1; --i)
    //         {
    //             startBonedir = glm::vec3(inCharLocalRot[i-1] * glm::vec4(_boneVector[_eyeBoneIndex[i-1]]._direction,1));
    //             float r = glm::length(inCharLocalPos[i] - inCharLocalPos[i-1]);
    //             float k = distance[i-1] / r;
    //             inCharLocalPos[i-1] = glm::mix(inCharLocalPos[i], inCharLocalPos[i-1], k);
    //             positionFixLimitAngle(inCharLocalPos[i-1], inCharLocalPos[i], startBonedir,_boneVector[_eyeBoneIndex[i]], true);
    //         }

    //         inCharLocalPos.front() = start;
    //         for(uint16 i = 0; i < _eyeBoneIndex.size()-1; ++i)
    //         {
    //             startBonedir = glm::vec3((inCharLocalRot[i] * glm::vec4(_boneVector[_eyeBoneIndex[i]]._direction,1)));
    //             float r = glm::length(inCharLocalPos[i] - inCharLocalPos[i+1]);
    //             float k = distance[i] / r;
    //             inCharLocalPos[i+1] = glm::mix(inCharLocalPos[i], inCharLocalPos[i+1], k);
    //             positionFixLimitAngle(inCharLocalPos[i], inCharLocalPos[i+1], startBonedir ,_boneVector[_eyeBoneIndex[i+1]], false);
    //         }
    //     }
    // }
    
    for (uint32 i = 0; i < _eyeBoneIndex.size(); ++i)
    {
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), inCharLocalPos[i]);
        _IKTranspos[_eyeBoneIndex[i]] = translate * inCharLocalRot[i];
    }
    return _IKTranspos;
}

void EyeIK::positionFixLimitAngle(glm::vec3& start, glm::vec3& end, const glm::quat& startBonedir, const Bone& endBone, bool backOrFront)
{
    glm::vec3 initialDirection = startBonedir * endBone._direction;
    glm::vec3 targetDirection = glm::normalize(end - start);
    glm::quat rotation = glm::rotation(initialDirection, targetDirection);
    glm::vec3 eulerAngle = glm::eulerAngles(rotation);

    for (auto& limit : endBone._limits)
    {
        DOF dof;
        float min, max;
        std::tie(dof, min, max) = limit;
        if (dof == DOF::RX)
            eulerAngle.x = glm::clamp(eulerAngle.x, glm::radians(min), glm::radians(max));
        else if (dof == DOF::RY)
            eulerAngle.y = glm::clamp(eulerAngle.y, glm::radians(min), glm::radians(max));
        else if (dof == DOF::RZ)
            eulerAngle.z = glm::clamp(eulerAngle.z, glm::radians(min), glm::radians(max));
    }

    rotation = glm::quat(eulerAngle);
    glm::vec3 correctPos = rotation * initialDirection * glm::length(end - start);

    if (backOrFront == true)
    {
        glm::vec3 startMove = start + correctPos;
        start = start + (end - startMove);
    }
    else 
    {
        end = start + correctPos;
    }
}
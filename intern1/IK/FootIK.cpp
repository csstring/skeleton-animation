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
    glm::vec3 normal(0, 1, -0.4);
    glm::vec3 groundPos(0,-10,0);
    // _groundVec = glm::normalize(groundPos - _targetPosition);
    glm::quat rot = glm::rotation(glm::vec3(1,0,0), groundPos - _targetPosition);
    //??
    _groundNormal = glm::normalize(normal);
    // _groundNormal = glm::cross(glm::cross(glm::normalize(_targetPosition- groundPos), normal), glm::normalize(_targetPosition));
    // std::cout << "target : " << glm::to_string(_targetPosition) << std::endl;
    // std::cout << "nomal : " << glm::to_string(_groundNormal) << std::endl;
}

void FootIK::setGroundNormal(glm::vec3 normal)
{
    _groundNormal = normal;
}

void FootIK::solveIK(
    std::vector<BoneLocal>& _boneLocalVector, 
    const glm::mat4& worldRotation, 
    const glm::mat4& worldTranslate, 
    const Controller& controller,
    const std::chrono::steady_clock::time_point& curTime)
{
    // findGround();
    this->_blendingRatio = 1;//default
    std::vector<glm::vec3> inCharLocalPos;
    std::vector<glm::mat4> inCharLocalRot;
    std::vector<glm::mat4> inCharTrans;
    glm::vec3 targetPosInBoneLocal;
    std::cout << glm::to_string(_groundNormal) << std::endl;
    for (uint32 i : _boneIndexVec)
    {
        inCharTrans.push_back(controller.getMatrixInCharLocal(i, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector));
        if (i == BONEID::RTIBIA)
            targetPosInBoneLocal = glm::inverse(inCharTrans.back()) * glm::inverse(worldTranslate * worldRotation) * glm::vec4(_targetPosition,1);
        inCharLocalPos.push_back(inCharTrans.back() * glm::vec4(0,0,0,1));
        inCharLocalRot.push_back(glm::mat3(inCharTrans.back()));
    }

    if (_isFirst == true)
    {
        _isFirst = false;
        _callTime = curTime;
    }

    glm::mat4 charLocalToWorld = worldTranslate * worldRotation;
    // glm::vec3 targetPosInCharLocal = glm::inverse(charLocalToWorld) * glm::vec4(_targetPosition, 1);
    glm::vec3 targetPosInCharLocal = inCharLocalPos[3];
    glm::mat4 trans3, trans2, trans1, trans0;
    //3
    
        glm::vec3 boneDir = _boneVector[_boneIndexVec[3]]._direction;
        glm::vec3 parentDir = glm::normalize(inCharLocalPos[2] - inCharLocalPos[3]);
        glm::vec3 axis = glm::normalize(glm::cross(_groundNormal, parentDir));
        glm::vec3 groundDir = glm::normalize(glm::cross(axis, _groundNormal));
        glm::quat Rot3 = glm::rotation(boneDir, groundDir);
        glm::vec3 Pos3InChar = targetPosInCharLocal;
        glm::quat Rot3InChar = glm::rotation(_boneVector[_boneIndexVec[3]]._direction, groundDir);
        // _boneLocalVector[_boneIndexVec[3]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[3]].translationInBoneLocal,Pos3InBone, _blendingRatio);
        // _boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal, Rot3, _blendingRatio);
    //2
    
        float distance2 = glm::length(inCharLocalPos[2] - inCharLocalPos[3]);//
        glm::vec3 Pos2InChar = Pos3InChar + groundDir * distance2;
        glm::vec3 Pos2InBone = glm::inverse(inCharTrans[1]) * glm::vec4(Pos2InChar, 1);
    //1

        float distance1 = glm::length(inCharLocalPos[1] - inCharLocalPos[2]);
        glm::vec3 Dir1 = glm::normalize(inCharLocalPos[1] - inCharLocalPos[2]); 
        glm::vec3 Pos1InChar = Dir1 * distance1 + Pos2InChar;
        glm::vec3 Pos1InBone = glm::inverse(inCharTrans[0]) * glm::vec4(Pos1InChar, 1);
        _boneLocalVector[_boneIndexVec[1]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[1]].translationInBoneLocal,Pos1InBone, _blendingRatio);
    
    //0
        float distance0 = glm::length(inCharLocalPos[0] - inCharLocalPos[1]);
        glm::vec3 Dir0 = glm::normalize(inCharLocalPos[0] - inCharLocalPos[1]); 
        glm::vec3 Pos0InChar = Dir0 * distance0 + Pos1InChar;
    //
        trans3 = glm::translate(glm::mat4(1.0f), Pos3InChar) * inCharLocalRot[3];
        trans2 = glm::translate(glm::mat4(1.0f), Pos2InChar) * inCharLocalRot[2];
        trans1 = glm::translate(glm::mat4(1.0f), Pos1InChar) * inCharLocalRot[1];
        trans0 = glm::translate(glm::mat4(1.0f), Pos0InChar) * inCharLocalRot[0];

        glm::vec3 Pos3InBone = glm::inverse(trans2) * glm::vec4(targetPosInCharLocal, 1);
        Pos2InBone = glm::inverse(trans1) * glm::vec4(Pos2InChar, 1);
        Pos1InBone = glm::inverse(trans0) * glm::vec4(Pos1InChar, 1);
        _boneLocalVector[_boneIndexVec[3]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[3]].translationInBoneLocal,Pos3InBone, _blendingRatio);
        _boneLocalVector[_boneIndexVec[2]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[2]].translationInBoneLocal,Pos2InBone, _blendingRatio);
        
        //rot
        glm::vec3 DirInBone3 = glm::inverse(trans2) * glm::vec4(Pos3InChar - Pos2InChar,0);
        glm::quat RotInBone3 = glm::rotation(_boneVector[_boneIndexVec[3]]._direction ,DirInBone3);
        _boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal, RotInBone3, _blendingRatio);
        
        // glm::vec3 DirInBone1 = glm::inverse(inCharTrans[0]) * glm::vec4(Pos1InChar - inCharLocalPos[0],0);
        // glm::quat RotInBone1 = glm::rotation(_boneVector[_boneIndexVec[1]]._direction ,DirInBone1);
        // _boneLocalVector[_boneIndexVec[1]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[1]].rotationInBoneLocal, RotInBone1, _blendingRatio);
}

/*
#include "include/EyeIK.h"
#include "include/Bone.h"
#include <algorithm>
#include "include/GLM/gtx/quaternion.hpp"
#include "include/GLM/gtc/quaternion.hpp"
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

const std::vector<glm::mat4>& EyeIK::solveEyeIK(const std::vector<glm::mat4>& characterTranspos, const glm::mat4& worldRotation)
{
    std::vector<glm::vec3> IKpos;
    std::vector<glm::mat4> IKrot;
    std::vector<float> distance;
    glm::vec3 headPosition;
    for (auto& it : _IKTranspos)
        it = glm::mat4(0.0f);
    for (uint32 i : _eyeBoneIndex)
    {
        IKpos.push_back(characterTranspos[i] * glm::vec4(0,0,0,1));
        IKrot.push_back(glm::mat3(characterTranspos[i]));
    }
    //rotation
    //cursee를 살짝 아래보게하면 자연스러울듯
    //init head target
    glm::vec3 tmptarget = glm::inverse(characterTranspos[_eyeBoneIndex.back()]) * glm::vec4(_targetPosition,1);
    for (uint32 i = _bonedirection.size()-1; i < _bonedirection.size(); ++i)
    {
        glm::vec3 targetDir = _targetPosition - IKpos[i];
        glm::vec3 headTopPos = IKrot[i] * glm::vec4(_bonedirection[i],1);
        glm::vec3 curSee = IKrot[i] * glm::vec4(glm::cross(glm::vec3(1,0,0), _bonedirection[i]),1);
        glm::quat afterSee = glm::rotation(glm::normalize(curSee), glm::normalize(targetDir));
        glm::vec3 headMove = afterSee * headTopPos;
        IKrot[i] = glm::toMat4(afterSee) * IKrot[i];

        float dis = glm::length(IKpos[i] - IKpos[i-2]);
        headPosition.x = IKpos[i-2].x + headMove.x * dis;
        headPosition.y = IKpos[i-2].y + headMove.y * dis;
        headPosition.z = IKpos[i-2].z + headMove.z * dis;
    }
    //IKrot[_bonedirection.size()-2] = glm::mix(IKrot[_bonedirection.size()-1], IKrot[_bonedirection.size()-3],0.5);
    //if reachable position
    //postion IK
    if (reachable(characterTranspos, distance, headPosition) == true)//distance 구하는걸로 수정, fix me
    {
        glm::vec3 start = IKpos.front();
        uint32 iterCount = 0;
        while (glm::length(headPosition - IKpos.back()) > 0.1)
        {
            iterCount++;
            if (iterCount >= 10)
            {
                return _IKTranspos;
            }
            IKpos.back() = headPosition;
            glm::quat startBonedir;
            for(uint16 i = _eyeBoneIndex.size()-1; i >=1; --i)
            {
                startBonedir = glm::vec3(IKrot[i-1] * glm::vec4(_boneVector[_eyeBoneIndex[i-1]]._direction,1));
                float r = glm::length(IKpos[i] - IKpos[i-1]);
                float k = distance[i-1] / r;
                IKpos[i-1] = glm::mix(IKpos[i], IKpos[i-1], k);
                positionFixLimitAngle(IKpos[i-1], IKpos[i], startBonedir,_boneVector[_eyeBoneIndex[i]], true);
            }

            IKpos.front() = start;
            for(uint16 i = 0; i < _eyeBoneIndex.size()-1; ++i)
            {
                startBonedir = glm::vec3((IKrot[i] * glm::vec4(_boneVector[_eyeBoneIndex[i]]._direction,1)));
                float r = glm::length(IKpos[i] - IKpos[i+1]);
                float k = distance[i] / r;
                IKpos[i+1] = glm::mix(IKpos[i], IKpos[i+1], k);
                positionFixLimitAngle(IKpos[i], IKpos[i+1], startBonedir ,_boneVector[_eyeBoneIndex[i+1]], false);
            }
        }
    }
    //fix me rot 
    glm::vec3 curSee = IKrot[_bonedirection.size()-2] * glm::vec4(_bonedirection[_bonedirection.size()-2],1);
    glm::quat afterSee = glm::rotation(glm::normalize(curSee), glm::normalize(IKpos[_bonedirection.size()-2] - IKpos[_bonedirection.size()-3]));
    IKrot[_bonedirection.size()-2] = glm::toMat4(afterSee) * IKrot[_bonedirection.size()-2];
    
    for (uint32 i = 0; i < _eyeBoneIndex.size(); ++i)
    {
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), IKpos[i]);
        _IKTranspos[_eyeBoneIndex[i]] = translate * IKrot[i];
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
*/
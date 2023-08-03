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
        curDistance += distance.back();
        start = end;
    }
    return true;
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

    std::vector<glm::vec3> inCharLocalPos, copyPos;
    std::vector<glm::mat4> inCharLocalRot;
    std::vector<glm::mat4> inCharTrans;
    glm::vec3 targetPosInBoneLocal;
    std::vector<float> distance;
    std::cout << glm::to_string(_groundNormal) << std::endl;
    for (uint32 i : _boneIndexVec)
    {
        inCharTrans.push_back(controller.getMatrixInCharLocal(i, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector));
        inCharLocalPos.push_back(inCharTrans.back() * glm::vec4(0,0,0,1));
        inCharLocalRot.push_back(glm::mat3(inCharTrans.back()));
    }

        glm::mat4 charLocalToWorld = worldTranslate * worldRotation;
        glm::vec3 targetPosInCharLocal = glm::inverse(charLocalToWorld) * glm::vec4(_targetPosition, 1);
        ////////////////////////////////////////////
        _groundNormal = glm::inverse(charLocalToWorld) * glm::vec4(_groundNormal,0);
        glm::vec3 parentDir = glm::normalize(inCharLocalPos[2] - inCharLocalPos[3]);
        glm::vec3 axis = glm::normalize(glm::cross(_groundNormal, parentDir));
        glm::vec3 groundDir = glm::normalize(glm::cross(axis, _groundNormal));
        glm::vec3 Pos3InChar = inCharLocalPos[3];//tmp

        float distance2 = glm::length(inCharLocalPos[2] - inCharLocalPos[3]);
        glm::vec3 footPosInChar = Pos3InChar + groundDir * distance2;

    if (reachable(inCharLocalPos, distance, footPosInChar) == true)//distance 구하는걸로 수정, fix me
    {
        glm::vec3 start = inCharLocalPos.front();
        uint32 iterCount = 0;
        while (glm::length(footPosInChar - inCharLocalPos[2]) > 0.1)
        {
            iterCount++;
            if (iterCount >= 10)
            {
                break;
            }
            inCharLocalPos[2] = footPosInChar;
            for(uint16 i = _boneIndexVec.size()-2; i >=1; --i)
            {
                float r = glm::length(inCharLocalPos[i] - inCharLocalPos[i-1]);
                float k = distance[i-1] / r;
                inCharLocalPos[i-1] = glm::mix(inCharLocalPos[i], inCharLocalPos[i-1], k);
            }

            inCharLocalPos.front() = start;
            for(uint16 i = 0; i < _boneIndexVec.size()-2; ++i)
            {
                float r = glm::length(inCharLocalPos[i] - inCharLocalPos[i+1]);
                float k = distance[i] / r;
                inCharLocalPos[i+1] = glm::mix(inCharLocalPos[i], inCharLocalPos[i+1], k);
            }
        }
    }
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

    glm::vec3 bonePos3, bonePos2, bonePos1, bonePos0;
    glm::quat boneRot3, boneRot2, boneRot1, boneRot0;

    bonePos3 = glm::inverse(trans2) * glm::vec4(inCharLocalPos[3], 1);
    bonePos2 = glm::inverse(trans1) * glm::vec4(inCharLocalPos[2], 1);
    bonePos1 = glm::inverse(trans0) * glm::vec4(inCharLocalPos[1], 1);
    bonePos0 = inCharLocalPos[0];//??

    boneRot3 = glm::quat(glm::inverse(trans2) * inCharLocalRot[3]);
    boneRot2 = glm::quat(glm::inverse(trans1) * inCharLocalRot[2]);
    boneRot1 = glm::quat(glm::inverse(trans0) * inCharLocalRot[1]);
    boneRot0 = inCharLocalRot[0];//??

    _boneLocalVector[_boneIndexVec[3]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[3]].translationInBoneLocal,bonePos3, _blendingRatio);
    _boneLocalVector[_boneIndexVec[2]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[2]].translationInBoneLocal,bonePos2, _blendingRatio);
    _boneLocalVector[_boneIndexVec[1]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[1]].translationInBoneLocal,bonePos1, _blendingRatio);
    // _boneLocalVector[_boneIndexVec[0]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[0]].translationInBoneLocal,bonePos0, _blendingRatio);

    _boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal, boneRot3, _blendingRatio);
    _boneLocalVector[_boneIndexVec[2]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[2]].rotationInBoneLocal, boneRot2, _blendingRatio);
    _boneLocalVector[_boneIndexVec[1]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[1]].rotationInBoneLocal, boneRot1, _blendingRatio);
    // _boneLocalVector[_boneIndexVec[0]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[0]].rotationInBoneLocal, boneRot0, _blendingRatio);


}

/*
const std::vector<glm::mat4>& EyeIK::solveEyeIK(const std::vector<glm::mat4>& characterTranspos, const glm::mat4& worldRotation)
{

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
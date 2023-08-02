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
    //외적으로 nomal다시 구하기
    //디폴트값 넣어준거라 추후 수정
    glm::vec3 normal(-1, 0., 0);
    glm::vec3 groundPos(0,-10,0);
    // _groundVec = glm::normalize(groundPos - _targetPosition);
    glm::quat rot = glm::rotation(glm::vec3(1,0,0), groundPos - _targetPosition);
    //??
    _groundNomal = normal;
    // _groundNomal = glm::cross(glm::cross(glm::normalize(_targetPosition- groundPos), normal), glm::normalize(_targetPosition));
    // std::cout << "target : " << glm::to_string(_targetPosition) << std::endl;
    // std::cout << "nomal : " << glm::to_string(_groundNomal) << std::endl;
}

void FootIK::solveIK(
    std::vector<BoneLocal>& _boneLocalVector, 
    const glm::mat4& worldRotation, 
    const glm::mat4& worldTranslate, 
    const Controller& controller,
    const std::chrono::steady_clock::time_point& curTime)
{
    findGround();
    this->_blendingRatio = 1;//default
    std::vector<glm::vec3> inCharLocalPos;
    std::vector<glm::mat4> inCharLocalRot;
    std::vector<glm::mat4> inCharTrans;
    glm::vec3 targetPosInBoneLocal;
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
    glm::vec3 targetPosInCharLocal = glm::inverse(charLocalToWorld) * glm::vec4(_targetPosition, 1);
    
    glm::mat4 trans3, trans2, trans1, trans0;
    //3
    
        glm::vec3 boneDir = _boneVector[_boneIndexVec[3]]._direction;
        glm::vec3 parentDir = glm::normalize(inCharLocalPos[2] - inCharLocalPos[3]);
        glm::vec3 groundDir = glm::cross(glm::cross(_groundNomal, parentDir), _groundNomal);
        glm::quat Rot3 = glm::rotation(boneDir, groundDir);
        glm::vec3 Pos3InChar = targetPosInCharLocal;
    
        // _boneLocalVector[_boneIndexVec[3]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[3]].translationInBoneLocal,Pos3InBone, _blendingRatio);
        // _boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal, Rot3, _blendingRatio);
    
    //2
    
        float distance2 = glm::length(inCharLocalPos[2] - inCharLocalPos[3]);//
        std::cout << distance2 << std::endl;
        glm::vec3 Pos2InChar = groundDir * distance2 + Pos3InChar;
        // _boneLocalVector[_boneIndexVec[2]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[2]].translationInBoneLocal,Pos2InBone, _blendingRatio);
    
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
        glm::vec3 Pos2InBone = glm::inverse(trans1) * glm::vec4(Pos2InChar, 1);
        Pos1InBone = glm::inverse(trans0) * glm::vec4(Pos1InChar, 1);
        _boneLocalVector[_boneIndexVec[3]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[3]].translationInBoneLocal,Pos3InBone, _blendingRatio);
        _boneLocalVector[_boneIndexVec[2]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[2]].translationInBoneLocal,Pos2InBone, _blendingRatio);
        // _boneLocalVector[_boneIndexVec[1]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[1]].translationInBoneLocal,Pos1InBone, _blendingRatio);
        // glm::vec3 Pos1InBone = glm::inverse(inCharTrans[0]) * glm::vec4(Pos1InChar, 1);
}
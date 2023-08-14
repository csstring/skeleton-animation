#include "../include/IK/FootIk.h"
#include "../include/GLM/gtx/quaternion.hpp"
#include "../include/GLM/gtc/quaternion.hpp"
#include "../include/Controller.h"
#include "../include/Character.h"
#include "../include/Bone.h"
#include "../include/IK/IKUtility.h"
//ground찾는건 월드 에서 하고 찾았으면 캐릭터 로컬에서 계산
//0 rhipjoint 1 rfemur 2 rtibia 3 rfoot
/*
목표를 찾았다면
target = true;
offTheGround == true;

목표 위치에 도달(blend ratio == 1)이 되면 
target = false 가되고
offTheGround = false 이므로

ik 시작되는 시점은
target == true;
offTheGround == true;
*/

const int MAX_HITS = 8;
//발바닥 조인트 두개 다 검사
void FootIK::setCharGroundHight(float& charGroundHight)
{
    charGroundHight = _groundHight;
}

bool FootIK::isAnimationBlendingOn(void)
{
    if (_targetOn == false && _blendingRatio <= 0 && _isRootAnimationOn == false)
        return false;
    return true;
}

void FootIK::setTestOption(glm::vec3 position, glm::vec3 normal, glm::mat4 charLocalToWorld)
{
    _targetOn = true;
    _targetPosition = glm::inverse(charLocalToWorld) * glm::vec4(position,1);
    _groundNormal = glm::inverse(charLocalToWorld) * glm::vec4(normal,0);
}

bool FootIK::isOffGroundCheck(
    const std::vector<glm::vec3>& inCharLocalPos, 
    physx::PxScene* gScene, 
    glm::mat4 charLocalToWorld)
{
    bool curIsOffGround;
    bool beforeIsOffGround = _isOffGround;
    glm::vec3 footPos = charLocalToWorld * glm::vec4(inCharLocalPos[3],1);
    glm::vec3 tibiaPos = charLocalToWorld * glm::vec4(inCharLocalPos[2],1);
    glm::vec3 femurPos = charLocalToWorld * glm::vec4(inCharLocalPos[1],1);

    glm::vec3 startPos = glm::mix(footPos, tibiaPos, 0.5f);
    startPos.y +=5;
    physx::PxSweepBuffer hit;
    physx::PxVec3 sweepDirection(0,-1,0);
    physx::PxTransform initialPose(physx::PxVec3(startPos.x, startPos.y, startPos.z));
    physx::PxSphereGeometry sphere(0.1);

    bool hitFlag = gScene->sweep(sphere, initialPose, sweepDirection, 10, hit);
    
    if (hitFlag == false || glm::abs(5 - hit.block.distance) >= 0.2)
        curIsOffGround = true;
    else
        curIsOffGround = false;

    _isOffGround = curIsOffGround;
    //발이 땅에서 떨어짐
    if (curIsOffGround == true)
    {
        // std::cout << "foot pos" << glm::to_string(startPos) << std::endl;
        // std::cout << "hit pos " << ft_to_string(hit.block.position) << std::endl;
        // std::cout << "hit dis : " << hit.block.distance << std::endl;
        // std::cout << "target pos " << glm::to_string(_targetPosition) << std::endl;
        // std::cout << "foot off start" << std::endl;
        return true;
    }
    return false;
}

bool FootIK::isStartFindTarget(const std::vector<glm::vec3>& inCharLocalPos)
{
    if (inCharLocalPos[2].y - inCharLocalPos[3].y > 0.98)
        return true;
    return false;
}

const float predicSecond = 0.25;

bool FootIK::findTargetObject(
    const std::vector<glm::vec3>& inCharLocalPos, 
    physx::PxScene* gScene, 
    glm::mat4 charLocalToWorld,
    glm::vec3 tmpMoveDir
)
{
    glm::vec3 footPos = charLocalToWorld * glm::vec4(inCharLocalPos[3],1);
    glm::vec3 tibiaPos = charLocalToWorld * glm::vec4(inCharLocalPos[2],1);

    float moveDistance = _velocity * predicSecond;
    glm::vec3 targetPos = footPos + tmpMoveDir;
    // std::cout << "find dis:" << glm::length(moveDistance * tmpMoveDir) << std::endl;
    // std::cout << "foot length : " << glm::length(footPos - tibiaPos) << std::endl;
    targetPos.y += 5;//fix

    physx::PxSweepBuffer hit;
    physx::PxVec3 sweepDirection(0,-1,0);
    physx::PxTransform initialPose(physx::PxVec3(targetPos.x, targetPos.y, targetPos.z));
    physx::PxSphereGeometry sphere(glm::length(footPos - tibiaPos) / 5);
    bool hitFlag = gScene->sweep(sphere, initialPose, sweepDirection, 10, hit);
 
    if (hitFlag == false || glm::abs(5 - hit.block.distance) >= 3)
    {
        return false;
    }

    if (hit.block.actor != _curTouchBody || hit.block.position.y != _groundHight)
    {
        glm::vec3 worldTarget(hit.block.position.x, hit.block.position.y, hit.block.position.z);
        glm::vec3 worldNormal(hit.block.normal.x, hit.block.normal.y, hit.block.normal.z);
        std::vector<float> disVector;
        if (reachable(inCharLocalPos,disVector ,glm::inverse(charLocalToWorld) * glm::vec4(worldTarget,1)) == false)
        {
            return false;
        }
        _targetPosition = glm::inverse(charLocalToWorld) * glm::vec4(worldTarget,1);
        _groundNormal = glm::inverse(charLocalToWorld) * glm::vec4(worldNormal, 0);
        // std::cout << "world target pos : " << glm::to_string(worldTarget) << std::endl;
        // std::cout << "local target pos : " << glm::to_string(_targetPosition) << std::endl;
        // std::cout << "re wolrd : " << glm::to_string(charLocalToWorld * glm::vec4(_targetPosition,1));
        _targetOn = true;
        _groundHight = hit.block.position.y;
        _curTouchBody = hit.block.actor;
    }
    return true;
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
        return true;
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

void FootIK::fixBendingAngle(glm::vec3& start, glm::vec3& mid, glm::vec3& end)
{
    glm::vec3 angle1 = glm::normalize(start - mid);
    glm::vec3 angle2 = glm::normalize(end - mid);
    glm::quat bendRot = glm::rotation(angle1, angle2);

    if (glm::axis(bendRot).x < 0)
    {
        return;
    }
    angle1 = glm::normalize(mid - start);
    angle2 = glm::normalize(end - start);
    bendRot = glm::rotation(angle1, angle2);
    
    glm::vec3 dir = bendRot * angle2;
    mid = start + dir * glm::length(start - mid);

    angle1 = glm::normalize(start - mid);
    angle2 = glm::normalize(end - mid);
    bendRot = glm::rotation(angle1, angle2);
    if (glm::axis(bendRot).x > 0)
    {
        std::cout << "bend fix check" << std::endl;
        return;
    }
}

void FootIK::saveBlendingAnimation(std::vector<glm::vec3>& inCharLocalPos, std::vector<glm::mat4>& inCharLocalRot)
{
    std::vector<float> distance;
    // glm::vec3 parentDir = glm::normalize(inCharLocalPos[2] - inCharLocalPos[3]);
    // glm::vec3 axis = glm::normalize(glm::cross(_groundNormal, parentDir));
    // glm::vec3 groundDir = glm::normalize(glm::cross(axis, _groundNormal));
    glm::vec3 parentDir = glm::normalize(inCharLocalPos[2] - inCharLocalPos[3]);
    glm::quat rot = glm::rotation(_groundNormal, parentDir);
    glm::vec3 axis = glm::axis(rot);
    float radians = glm::radians(90.0f) - glm::angle(rot);
    glm::vec3 groundDir = glm::angleAxis(radians, axis) * parentDir;

    groundDir = glm::mix(parentDir, groundDir, _blendingRatio);
    float distance2 = glm::length(inCharLocalPos[2] - inCharLocalPos[3]);
    glm::vec3 tmpTarget = inCharLocalPos[3];
    tmpTarget.y = glm::mix(tmpTarget.y, _targetPosition.y, _blendingRatio);

    if (parentDir.y > 0.75)//normal 방향이랑 거의 같아지면 휙 돌아버림 fix me
    {
        std::cout << parentDir.y << std::endl;
        std::cout << glm::degrees(glm::angle(rot)) << std::endl;
        groundDir = parentDir;
    }

    glm::vec3 footPosInChar = tmpTarget + groundDir * distance2;
    if (reachable(inCharLocalPos, distance, footPosInChar) == false)//fix me 
    {

    }

    glm::vec3 toRootDir = -inCharLocalPos[2];
    
    glm::vec3 start = inCharLocalPos.front();
    uint32 iterCount = 0;
    inCharLocalPos[3] = tmpTarget;
    while (glm::length(footPosInChar - inCharLocalPos[2]) > 0.1)
    {
        iterCount++;
        if (iterCount >= 30)
        {
            // std::cout << "Fook ik length val : " << glm::length(footPosInChar - inCharLocalPos[2]) << std::endl;
            break;
        }
            
        inCharLocalPos[2] = footPosInChar;
        glm::vec3 BoneDir;
        for(uint16 i = _boneIndexVec.size()-2; i >=1; --i)
        {
            float r = glm::length(inCharLocalPos[i] - inCharLocalPos[i-1]);
            float k = distance[i-1] / r;
            inCharLocalPos[i-1] = glm::mix(inCharLocalPos[i], inCharLocalPos[i-1], k);
            if (i-1 == 1)
                fixBendingAngle(start, inCharLocalPos[1], footPosInChar);
            BoneDir = inCharLocalPos[i+1] - inCharLocalPos[i];
            // positionFixLimitAngleBackWard(inCharLocalPos[i-1], inCharLocalPos[i], BoneDir ,_boneVector[_boneIndexVec[i+1]]);
        }

        inCharLocalPos.front() = start;
        for(uint16 i = 0; i < _boneIndexVec.size()-2; ++i)//1 2 
        {
            float r = glm::length(inCharLocalPos[i] - inCharLocalPos[i+1]);
            float k = distance[i] / r;
            inCharLocalPos[i+1] = glm::mix(inCharLocalPos[i], inCharLocalPos[i+1], k);
            if (i+1 == 1)
                fixBendingAngle(start, inCharLocalPos[1], footPosInChar);
            if (i == 0)
                BoneDir = inCharLocalRot[i] * glm::vec4(_boneVector[_boneIndexVec[0]]._direction,0);
            else
                BoneDir = inCharLocalPos[i] - inCharLocalPos[i-1];
            positionFixLimitAngleForWard(inCharLocalPos[i], inCharLocalPos[i+1], BoneDir, _boneVector[_boneIndexVec[i+1]]);
        }
    }


    glm::vec3 inCharDir3 = glm::normalize(inCharLocalPos[3] - inCharLocalPos[2]);
    glm::vec3 inCharDir2 = glm::normalize(inCharLocalPos[2] - inCharLocalPos[1]);
    glm::vec3 inCharDir1 = glm::normalize(inCharLocalPos[1] - inCharLocalPos[0]);
    inCharLocalRot[3] = glm::toMat4(glm::rotation(_boneVector[_boneIndexVec[3]]._direction ,inCharDir3));
    inCharLocalRot[2] = glm::toMat4(glm::rotation(_boneVector[_boneIndexVec[2]]._direction ,inCharDir2));
    inCharLocalRot[1] = glm::toMat4(glm::rotation(_boneVector[_boneIndexVec[1]]._direction ,inCharDir1));

    glm::mat4 trans2, trans1, trans0;
    trans2 = glm::translate(glm::mat4(1.0f), inCharLocalPos[2]) * inCharLocalRot[2];
    trans1 = glm::translate(glm::mat4(1.0f), inCharLocalPos[1]) * inCharLocalRot[1];
    trans0 = glm::translate(glm::mat4(1.0f), inCharLocalPos[0]) * inCharLocalRot[0];

    _bonePos[3] = glm::inverse(trans2) * glm::vec4(inCharLocalPos[3], 1);
    _bonePos[2] = glm::inverse(trans1) * glm::vec4(inCharLocalPos[2], 1);
    _bonePos[1] = glm::inverse(trans0) * glm::vec4(inCharLocalPos[1], 1);

    _boneRot[3] = glm::quat(glm::inverse(trans2) * inCharLocalRot[3]);
    _boneRot[2] = glm::quat(glm::inverse(trans1) * inCharLocalRot[2]);
    _boneRot[1] = glm::quat(glm::inverse(trans0) * inCharLocalRot[1]);
    // std::cout << "target : "<<_targetPosition.y << std::endl;
    // std::cout << "front : "<<inCharLocalPos[3].y << std::endl;
    // std::cout << "back : "<<inCharLocalPos[2].y << std::endl;
    // std::cout << "blendint ratio : " << _blendingRatio << std::endl;
    // std::cout << "blending target : " << tmpTarget.y << std::endl;
    //root pos
    if (_bonePos[0].y == 0)
    {
        _bonePos[0].y = _targetPosition.y - inCharLocalPos[3].y;
    }

    //fix me

    _isSaveAnimation = true;
}

void FootIK::solveIK(
    std::vector<BoneLocal>& _boneLocalVector, 
    const glm::mat4& worldRotation, 
    const glm::mat4& worldTranslate, 
    const Controller& controller,
    const std::chrono::steady_clock::time_point& curTime,
    physx::PxScene* gScene
)
{
    _curTime = curTime;
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

    if (_isFirst == true)
    {
        _isFirst = false;
        _prevTime = _curTime;
        // inCharLocalPos = inCharLocalPos;
        // inCharLocalRot = inCharLocalRot;
    }

    //save velocity
    {
        glm::vec3 beforePos = charLocalToWorld * glm::vec4(0,0,0,1);
        glm::mat4 curRootTrans = controller.getMatrixInCharLocal(BONEID::ROOT, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector);
        glm::vec3 curPos = charLocalToWorld * curRootTrans * glm::vec4(0,0,0,1);
        saveVelocity(beforePos, curPos);
    }

    if (isStartFindTarget(inCharLocalPos) && _isRootAnimationOn == false && _blendingRatio <= 0)// || _retargetTime >= 1000)
    {   
        // std::cout << _velocity << std::endl;
        _bonePos[0].y = 0;
        glm::mat4 rootTrans = charLocalToWorld * controller.getMatrixInCharLocal(BONEID::ROOT, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector);
        glm::vec3 moveDir;

        moveDir = rootTrans * glm::vec4(0,0,1,0);
        glm::vec3 foot1Pos = controller.getMatrixInCharLocal(BONEID::RFOOT, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector) * glm::vec4(0,0,0,1);
        glm::vec3 foot2Pos = controller.getMatrixInCharLocal(BONEID::LFOOT, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector) * glm::vec4(0,0,0,1);
        moveDir = glm::length(foot1Pos - foot2Pos) * 1.8f * moveDir;
        if (findTargetObject(inCharLocalPos, gScene, charLocalToWorld, moveDir) == true)
        {
                // std::cout <<"find new target" << std::endl;
        }
    }
    if (_retargetTime >= 1000)
        _retargetTime = 0;
    std::chrono::milliseconds  millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(_curTime - _prevTime);
    float tmpRatio = static_cast<float>(millisecond.count()) / 200.0f;
    _retargetTime += millisecond.count();

    blendingRatioUpdate();
    if (_blendingRatio <= 0.0f && _targetOn == false && _isRootAnimationOn == false)
    {
        return ;
    }

    saveBlendingAnimation(inCharLocalPos, inCharLocalRot);
    // if (_isSaveAnimation == false)
    //     return;

    if (_isRootAnimationOn == true)
    {
        _boneLocalVector[BONEID::ROOT].translationInBoneLocal = glm::mix(_boneLocalVector[BONEID::ROOT].translationInBoneLocal,_bonePos[0], tmpRatio);
        _rootRatio -= tmpRatio;
    }
    if (_blendingRatio > 0)
    {
        // inCharLocalPos = inCharLocalPos;
        // inCharLocalRot = inCharLocalRot;
        _boneLocalVector[_boneIndexVec[3]].translationInBoneLocal = _bonePos[3];
        _boneLocalVector[_boneIndexVec[2]].translationInBoneLocal = _bonePos[2];
        _boneLocalVector[_boneIndexVec[1]].translationInBoneLocal = _bonePos[1];

        _boneLocalVector[_boneIndexVec[3]].rotationInBoneLocal = _boneRot[3];
        _boneLocalVector[_boneIndexVec[2]].rotationInBoneLocal = _boneRot[2];
        _boneLocalVector[_boneIndexVec[1]].rotationInBoneLocal = _boneRot[1];
    }
}

void FootIK::blendingRatioUpdate(void)
{
    float millisecond = getMilisecondTime(_curTime ,_prevTime);
    if (_targetOn == false && _blendingRatio <= 0 && _rootRatio <= 0)
    {
        _prevTime = _curTime;
        _isSaveAnimation = false;
        _isRootAnimationOn = false;
        _rootRatio = 1;
        return;
    }
    else if (_targetOn == false && _blendingRatio > 0)
        _blendingRatio -= millisecond / 150.0f;
    else if (_targetOn == true && _blendingRatio < 1.0f)
        _blendingRatio += millisecond / 150.0f;
    if (_blendingRatio >= 1.0f)
    {
        _blendingRatio = 1;
        _targetOn = false;
        _isRootAnimationOn = true;
    } 
    else if (_blendingRatio < 0)
    {
        _blendingRatio = 0;
    }
    if (_rootRatio <= 0)
       _isRootAnimationOn = false;
    _prevTime = _curTime;
}
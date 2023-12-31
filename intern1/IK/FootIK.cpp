#include "../include/IK/FootIk.h"
#include "../include/GLM/gtx/quaternion.hpp"
#include "../include/GLM/gtc/quaternion.hpp"
#include "../include/Controller.h"
#include "../include/Character.h"
#include "../include/Bone.h"
#include "../include/IK/IKUtility.h"
#include "../include/Physx.h"
//발바닥 조인트 두개 다 검사
void FootIK::setCharGroundHight(float& charGroundHight)
{
    charGroundHight = _groundHight;
}

void FootIK::setTestOption(glm::vec3 position, glm::vec3 normal, glm::mat4 charLocalToWorld)
{
    _targetOn = true;
    _targetPosition = glm::inverse(charLocalToWorld) * glm::vec4(position,1);
    _groundNormal = glm::inverse(charLocalToWorld) * glm::vec4(normal,0);
}

void FootIK::isOffGroundCheck(
    const std::vector<glm::vec3>& inCharLocalPos, 
    Physx* physx, 
    glm::mat4 charLocalToWorld)
{
    bool curIsOffGround;
    glm::vec3 footPos = charLocalToWorld * glm::vec4(inCharLocalPos[3],1);
    glm::vec3 tibiaPos = charLocalToWorld * glm::vec4(inCharLocalPos[2],1);
    glm::vec3 femurPos = charLocalToWorld * glm::vec4(inCharLocalPos[1],1);

    glm::vec3 startPos = glm::mix(footPos, tibiaPos, 0.5f);
    startPos.y +=5;
    physx::PxSweepBuffer hit;
    physx::PxVec3 sweepDirection(0,-1,0);
    physx::PxTransform initialPose(physx::PxVec3(startPos.x, startPos.y, startPos.z));
    physx::PxSphereGeometry sphere(0.1);

    bool hitFlag = physx->gScene->sweep(sphere, initialPose, sweepDirection, 10, hit);
    
    if (hitFlag == false || glm::abs(5 - hit.block.distance) >= 0.3)
        _isOffGround = true;
    else
        _isOffGround = false;
}

bool FootIK::isStartFindTarget(const std::vector<glm::vec3>& inCharLocalPos)
{
    if (inCharLocalPos[2].y - inCharLocalPos[3].y > 0.98)
        return true;
    return false;
}


bool FootIK::findTargetObject(
    const std::vector<glm::vec3>& inCharLocalPos, 
    Physx* physx, 
    glm::mat4 charLocalToWorld,
    glm::vec3 tmpMoveDir
)
{
    glm::vec3 tibiaPos = charLocalToWorld * glm::vec4(inCharLocalPos[3],1);
    glm::vec3 targetPos;
    // if (tmpMoveDir == glm::vec3(0.0f))
    //     tibiaPos = charLocalToWorld * glm::vec4(_prevTibiaPos,1);
   
    targetPos = tibiaPos + tmpMoveDir;
    targetPos.y += 5;//fix

    physx::PxSweepBuffer hit;
    glm::vec3 sweepDirection(0,-1,0);
    float radius = 1.0f / 5.0f;
    bool hitFlag = physx->sweepTestUseSphere(20, radius, targetPos, sweepDirection, hit);

    if (hitFlag == false || glm::abs(5 - hit.block.distance) >= 5)
    {
        // if (hitFlag == false)
        //     // std::cout << "hit fail" << std::endl;
        // else
            // std::cout << "distance fail : " << glm::abs(5-hit.block.distance) << std::endl;
        return false;
    }
    
    glm::vec3 worldTarget(hit.block.position.x, hit.block.position.y, hit.block.position.z);
    glm::vec3 worldNormal(hit.block.normal.x, hit.block.normal.y, hit.block.normal.z);
    std::vector<float> disVector;
    stairDir tmpDir;
        
    if (worldTarget.y > tibiaPos.y + 0.1)
        tmpDir = stairDir::UPSTAIR;
    else if (worldTarget.y < tibiaPos.y - 0.1)
        tmpDir = stairDir::DOWNSTAIR;
    else
        tmpDir = stairDir::NORMAL;

    //reach fail
    if (reachable(inCharLocalPos,disVector ,glm::inverse(charLocalToWorld) * glm::vec4(worldTarget,1)) == false && tmpDir != stairDir::DOWNSTAIR)
        return false;

    if (tmpMoveDir != glm::vec3(0.0f))
        _stairDir = tmpDir;
            
    _targetPosition = glm::inverse(charLocalToWorld) * glm::vec4(worldTarget,1);
    _groundNormal = glm::inverse(charLocalToWorld) * glm::vec4(worldNormal, 0);
    _targetOn = true;
    _groundHight = hit.block.position.y;
    _curTouchBody = hit.block.actor;
    
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
        return;

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
        // std::cout << "bend fix check" << std::endl;
        return;
    }
}

bool FootIK::saveBlendingAnimation(std::vector<glm::vec3>& inCharLocalPos, std::vector<glm::mat4>& inCharLocalRot, glm::vec3 curFootPos)
{
    std::vector<float> distance;
    glm::vec3 parentDir = glm::normalize(inCharLocalPos[2] - inCharLocalPos[3]);
    glm::quat rot = glm::rotation(_groundNormal, parentDir);
    glm::vec3 axis = glm::axis(rot);
    float radians = glm::radians(90.0f) - glm::angle(rot);
    glm::vec3 groundDir = glm::angleAxis(radians, axis) * parentDir;
    groundDir = glm::mix(parentDir, groundDir, _blendingRatio);

    glm::vec3 tmpTarget = curFootPos;
    if (_targetPosition.y > _firstHitHight)
        tmpTarget.y = glm::mix(tmpTarget.y, _targetPosition.y, _blendingRatio);
    else
        tmpTarget.y = glm::mix(tmpTarget.y, _firstHitHight, _blendingRatio);

    if (_bonePos[0].y == 0)
        _bonePos[0].y = _targetPosition.y - inCharLocalPos[3].y;
    glm::vec3 footPosInChar = tmpTarget + groundDir * glm::length(inCharLocalPos[2] - inCharLocalPos[3]);
    
    if (reachable(inCharLocalPos, distance, footPosInChar) == false)//fix me 
    {}
    
    glm::vec3 start = inCharLocalPos.front();
    uint32 iterCount = 0;
    while (glm::length(footPosInChar - inCharLocalPos[2]) > 0.1)
    {
        iterCount++;
        if (iterCount >= 30)
        {
            // std::cout << "Fook ik length val : " << glm::length(footPosInChar - inCharLocalPos[2]) << std::endl;
            return false;
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

    inCharLocalPos[3] = tmpTarget;
    for (int i = 3; i >= 1; --i)
    {
        glm::vec3 inCharDir = glm::normalize(inCharLocalPos[i] - inCharLocalPos[i-1]);
        inCharLocalRot[i] = glm::toMat4(glm::rotation(_boneVector[_boneIndexVec[i]]._direction ,inCharDir));
  
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), inCharLocalPos[i-1]) * inCharLocalRot[i-1];
        _bonePos[i] = glm::inverse(trans) * glm::vec4(inCharLocalPos[i], 1);
        _boneRot[i] = glm::quat(glm::inverse(trans) * inCharLocalRot[i]);
    }

    return true;
}

void FootIK::solveIK(
    std::vector<BoneLocal>& _boneLocalVector, 
    const glm::mat4& worldRotation, 
    const glm::mat4& worldTranslate, 
    const Controller& controller,
    const std::chrono::steady_clock::time_point& curTime,
    LowerState curState,
    Physx* physx
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
    }
    //save velocity
    {
        glm::vec3 beforePos = charLocalToWorld * glm::vec4(0,0,0,1);
        glm::mat4 curRootTrans = controller.getMatrixInCharLocal(BONEID::ROOT, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector);
        glm::vec3 curPos = charLocalToWorld * curRootTrans * glm::vec4(0,0,0,1);
        saveVelocity(beforePos, curPos);
    }

    //get delta time, prevTime update
    {
        _deltaMilisecond = getMilisecondTime(_curTime ,_prevTime);
        _prevTime = _curTime;
    }

    if (isStartFindTarget(inCharLocalPos) && _isRootAnimationOn == false && _rootRatio <= 0)// || _retargetTime >= 1000)
    {   
        glm::mat4 rootTrans = charLocalToWorld * controller.getMatrixInCharLocal(BONEID::ROOT, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector);
        glm::vec3 foot1Pos = controller.getMatrixInCharLocal(BONEID::RFOOT, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector) * glm::vec4(0,0,0,1);
        glm::vec3 foot2Pos = controller.getMatrixInCharLocal(BONEID::LFOOT, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector) * glm::vec4(0,0,0,1);
        foot1Pos.y = 0;
        foot2Pos.y = 0;
        glm::vec3 moveDir = glm::length(foot1Pos - foot2Pos) * 1.6f *  glm::normalize(rootTrans * glm::vec4(0,0,1,0));

        if (findTargetObject(inCharLocalPos, physx, charLocalToWorld, moveDir) == true)
        {
            _isMoveAnimation = true;
            _bonePos[0].y = 0;
            _rootRatio = 1;
            _blendingRatio = 0;
            _isRootAnimationOn = false;
        }
    }
    else if (_isMoveAnimation == false)
    {
        if (findTargetObject(inCharLocalPos, physx, charLocalToWorld, glm::vec3(0.0f)) == true)
        {
            _bonePos[0].y = 0;
            _rootRatio = 0;
            _isRootAnimationOn = false;
            // if (curState == LowerState::IDLE)
            // {
            //     _rootRatio = 1;
            //     _isRootAnimationOn = true;
            // }
        }
    }

    if (_retargetTime >= 1000)
        _retargetTime = 0;
    _retargetTime += _deltaMilisecond;

    if (_blendingRatio <= 0.0f && _targetOn == false && _isRootAnimationOn == false && _rootRatio <= 0)
        return ;

    blendingRatioUpdate();

    _firstHitHight = getFirstHitHight(charLocalToWorld, inCharLocalPos[3], physx);
    bool saveAnimation = saveBlendingAnimation(inCharLocalPos, inCharLocalRot, inCharLocalPos[3]);

    //debug
    if (saveAnimation == false)
    {
        // std::cout <<"save fail" << std::endl;
        // std::cout << _bonePos[0].y << std::endl;
        // std::cout << "_stairDir " << (uint32)_stairDir << std::endl;
        // std::cout << "target pos" << glm::to_string(_targetPosition) << std::endl;
        // std::cout <<"is root animation " << _isRootAnimationOn << std::endl;
        // std::cout <<"rootRatio " << _rootRatio << std::endl;
        // std::cout <<"_blendingRatio" << _blendingRatio << std::endl;
        // std::cout <<"_isMoveAnimation" << _isMoveAnimation << std::endl;
        // std::cout <<"_targetOn " << _targetOn << std::endl;
    } else {
        // std::cout << "foot pos" << glm::to_string(charLocalToWorld * glm::vec4(inCharLocalPos[2],1)) << std::endl;
        // std::cout << "tibia pos" << glm::to_string(charLocalToWorld * glm::vec4(inCharLocalPos[3],1)) << std::endl;
        // std::cout << "target pos" << glm::to_string(charLocalToWorld * glm::vec4(_targetPosition,1)) << std::endl;
        // std::cout <<"_targetOn " << _targetOn << std::endl;
        // std::cout <<"_blendingRatio" << _blendingRatio << std::endl;
    }

    _prevTibiaPos = inCharLocalPos[3];
    if (_isRootAnimationOn == true || (_stairDir == stairDir::DOWNSTAIR && _rootRatio > 0))
    {
        float rootRatioDelta = _deltaMilisecond / 200.0f;
        if (rootRatioDelta > _rootRatio)
            rootRatioDelta = _rootRatio;
        _boneLocalVector[BONEID::ROOT].translationInBoneLocal = glm::mix(_boneLocalVector[BONEID::ROOT].translationInBoneLocal,_bonePos[0], rootRatioDelta);
        _rootRatio -= rootRatioDelta;
    }

    if (saveAnimation == true)
    {
        for (int i=3; i >=1; --i)
        {
            // std::cout << std::to_string(_boneIndexVec[i]) << " : " << glm::to_string(_bonePos[i]) << std::endl;
            // std::cout <<std::to_string(_boneIndexVec[i]) << " : " << glm::to_string(_boneLocalVector[_boneIndexVec[i]].translationInBoneLocal) << std::endl;
            _boneLocalVector[_boneIndexVec[i]].translationInBoneLocal = _bonePos[i];
            _boneLocalVector[_boneIndexVec[i]].rotationInBoneLocal = _boneRot[i];
        }
    }

    //move change
    if (_isMoveAnimation == true && _blendingRatio == 0 && _rootRatio == 0 && _targetOn == false && _isRootAnimationOn == false)
        _isMoveAnimation = false;
}

float FootIK::getFirstHitHight(const glm::mat4& charLocalToWorld, const glm::vec3& inCharPos, Physx* physx)
{
    physx::PxSweepBuffer hit;
    glm::vec3 sweepDirection(0,-1,0);
    glm::vec4 inWorldPos = charLocalToWorld * glm::vec4(inCharPos, 1);
    inWorldPos.y += 5;
    bool hitFlag = physx->sweepTestUseSphere(100, 0.3, inWorldPos, sweepDirection, hit);
    inWorldPos = glm::vec4(hit.block.position.x, hit.block.position.y, hit.block.position.z,1);

    glm::vec3 inCharHitPos = glm::inverse(charLocalToWorld) * inWorldPos;
    if (hitFlag == false)
        return -100000;
    else
        return inCharHitPos.y;
}

void FootIK::blendingRatioUpdate(void)
{
    if (_targetOn == false && _blendingRatio > 0)
        _blendingRatio -= _deltaMilisecond / 125.0f;
    else if (_targetOn == true && _blendingRatio < 1.0f)
        _blendingRatio += _deltaMilisecond / 250.0f;
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
}
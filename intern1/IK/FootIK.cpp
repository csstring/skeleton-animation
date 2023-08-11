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
    if (_targetOn == false && _blendingRatio <= 0)
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

    glm::vec3 parentDir = glm::normalize(tibiaPos - femurPos);
    glm::vec3 childDir = glm::normalize(footPos - tibiaPos);
    glm::vec3 axis = glm::normalize(glm::cross(parentDir, childDir));
    glm::vec3 Dir =  glm::normalize(glm::cross(childDir, axis));

    physx::PxVec3 lightDir(Dir.x, Dir.y, Dir.z);
    physx::PxReal maxDistance = 1;
    physx::PxRaycastBuffer hitTibia, hitFoot;
    const float Epsilon = 0;
    glm::vec3 epsilonDir = Dir * Epsilon;
    physx::PxVec3 pxTibiaPos(tibiaPos.x - epsilonDir.x, tibiaPos.y - epsilonDir.y, tibiaPos.z - epsilonDir.z);
    physx::PxVec3 pxFootPos(footPos.x - epsilonDir.x, footPos.y - epsilonDir.y, footPos.z - epsilonDir.z);


    physx::PxRaycastHit hitBuffer[MAX_HITS];
    physx::PxRaycastBuffer hitInfo(hitBuffer, MAX_HITS);
    
    physx::PxRaycastHit *tibiaHitActor = nullptr;
    physx::PxRaycastHit *footHitActor = nullptr;
    
    bool tibiaHit = gScene->raycast(pxTibiaPos, lightDir,maxDistance, hitInfo);
    for (physx::PxU32 i = 0; i < hitInfo.nbTouches; i++)
    {
        if (_curTouchBody == hitInfo.touches[i].actor)
        {
            tibiaHitActor = &hitInfo.touches[i];
            break;
        }
    }

    bool footHit = gScene->raycast(pxFootPos, lightDir,maxDistance, hitInfo);
    for (physx::PxU32 i = 0; i < hitInfo.nbTouches; i++)
    {
        if (_curTouchBody == hitInfo.touches[i].actor)
        {
            footHitActor = &hitInfo.touches[i];
            break;
        }
    }

    if (tibiaHitActor == nullptr || footHitActor == nullptr)
        curIsOffGround = true;
    else
        curIsOffGround = false;

    _isOffGround = curIsOffGround;
    //발이 땅에서 떨어짐
    if (curIsOffGround == true)
    {
        std::cout << "foot off start" << std::endl;
        _targetOn = false;
        return true;
    }
    return false;
}
//root trans -> 진행 방향
//왼발 오른발의 거리 차이 : 보폭
//대각선 거리라 내적으로 직선거리구하기?
//발바닥 위치 + 거리*방향 *2 구하고 레이케스트
//타겟 위치 노발 구하고 캐릭터 로컬로 들고오기

void FootIK::findTargetObject(
    const std::vector<glm::vec3>& inCharLocalPos, 
    physx::PxScene* gScene, 
    glm::mat4 charLocalToWorld
)
{
    glm::vec3 footPos = charLocalToWorld * glm::vec4(inCharLocalPos[3],1);
    glm::vec3 tibiaPos = charLocalToWorld * glm::vec4(inCharLocalPos[2],1);

    glm::vec3 moveDir = glm::normalize(footPos - tibiaPos);//fix, normal 
    glm::vec3 targetPos = footPos;
    targetPos.y += 1;//fix

    physx::PxVec3 lightDir(0, -1, 0);
    physx::PxReal maxDistance = 2;

    physx::PxRaycastHit hitBuffer[MAX_HITS];
    physx::PxRaycastBuffer hitInfo(hitBuffer, MAX_HITS);
    physx::PxRaycastHit* hit = nullptr;

    physx::PxVec3 origin(targetPos.x , targetPos.y, targetPos.z);

    if (gScene->raycast(origin, lightDir, maxDistance, hitInfo) == true)
    {
        float maxHight = -1000;
        for (physx::PxU32 i = 0; i < hitInfo.nbTouches; i++)
        {
            if ((&hitInfo.touches[i])->position.y > maxHight)
            {
                maxHight = (&hitInfo.touches[i])->position.y;
                hit = &hitInfo.touches[i];
            }
        }
        // std::cout << "inchar move dir " << glm::to_string(glm::vec4(tmpMoveDir,0)) << std::endl;
        // std::cout << "world foot pos : " << glm::to_string(glm::inverse(charLocalToWorld) * glm::vec4(footPos,1)) << std::endl;
        // std::cout << "local foot pos : " << glm::to_string(footPos) << std::endl;
        // std::cout << "world origin pos : " << glm::to_string(targetPos) << std::endl;
        // std::cout << "target world pos : " << glm::to_string(worldTarget) << std::endl;
        // std::cout << "ground world Normal : " << glm::to_string(worldNormal) << std::endl;
        // std::cout << "target local pos : " << glm::to_string(_targetPosition) << std::endl;
        // std::cout << "ground local Normal : " << glm::to_string(_groundNormal) << std::endl;
    }

    if (hit == nullptr)
        return;
    
    if (hit->actor != _curTouchBody || hit->position.y != _groundHight)
    {
        glm::vec3 worldTarget(hit->position.x, hit->position.y, hit->position.z);
        glm::vec3 worldNormal(hit->normal.x, hit->normal.y, hit->normal.z);
        std::vector<float> disVector;
        if (reachable(inCharLocalPos,disVector ,glm::inverse(charLocalToWorld) * glm::vec4(worldTarget,1)) == false)
            return;
        _targetPosition = glm::inverse(charLocalToWorld) * glm::vec4(worldTarget,1);
        _groundNormal = glm::inverse(charLocalToWorld) * glm::vec4(worldNormal, 0);
        
        _targetOn = true;
        _groundHight = hit->position.y;
        _curTouchBody = hit->actor;
    }
}
/*
reach test fail!!
leg length : 6.65117
target length : 7.09239
*/
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

    if (glm::angle(bendRot) >= glm::radians(180.0f))
        return;
    
    angle1 = glm::normalize(mid - start);
    angle2 = glm::normalize(end - start);
    bendRot = glm::rotation(angle1, angle2);
    
    glm::vec3 dir = bendRot * bendRot * angle1;
    mid = start + dir * glm::length(start - mid);
}

bool FootIK::saveBlendingAnimation(std::vector<glm::vec3>& inCharLocalPos, std::vector<glm::mat4>& inCharLocalRot)
{
    std::vector<float> distance;
    glm::vec3 parentDir = glm::normalize(inCharLocalPos[2] - inCharLocalPos[3]);
    glm::vec3 axis = glm::normalize(glm::cross(_groundNormal, parentDir));
    glm::vec3 groundDir = glm::normalize(glm::cross(axis, _groundNormal));

    float distance2 = glm::length(inCharLocalPos[2] - inCharLocalPos[3]);
    glm::vec3 footPosInChar = _targetPosition + groundDir * distance2;

    if (reachable(inCharLocalPos, distance, footPosInChar) == false)
    {

        _targetOn = false;
        _curTouchBody = nullptr;
        return false;
    }

    glm::vec3 start = inCharLocalPos.front();
    uint32 iterCount = 0;
    inCharLocalPos[3] = _targetPosition;
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
            // if (i+1 == 1)
            //     fixBendingAngle(start, inCharLocalPos[1], footPosInChar);
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

    _isSaveAnimation = true;
    return true;
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
        _callTime = curTime;
    }

    if (_targetOn == false && _blendingRatio <= 0 )
    {
        glm::vec3 origin = charLocalToWorld * glm::vec4(0,0,0,1);
        glm::mat4 rootTrans = charLocalToWorld * controller.getMatrixInCharLocal(BONEID::ROOT, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector);
        glm::vec3 rootPos = rootTrans * glm::vec4(0,0,0,1);

        findTargetObject(inCharLocalPos, gScene, charLocalToWorld);
    }

    blendingRatioUpdate(curTime);
    if (_blendingRatio <= 0.0f && _targetOn == false)
        return ;

    if (_isSaveAnimation == false && saveBlendingAnimation(inCharLocalPos, inCharLocalRot) == false)
        return;

    for (int i = 3; i >= 1; --i)
    {
        _boneLocalVector[_boneIndexVec[i]].translationInBoneLocal = glm::mix(_boneLocalVector[_boneIndexVec[i]].translationInBoneLocal,_bonePos[i], _blendingRatio);
        _boneLocalVector[_boneIndexVec[i]].rotationInBoneLocal = glm::slerp(_boneLocalVector[_boneIndexVec[i]].rotationInBoneLocal, _boneRot[i], _blendingRatio);
    }


    // inCharLocalPos.clear();
    // inCharLocalRot.clear();
    // inCharTrans.clear();
    // for (uint32 i : _boneIndexVec)
    // {
    //     inCharTrans.push_back(controller.getMatrixInCharLocal(i, controller.getPlayer()->getCharacterSkeleton(), _boneLocalVector));
    //     inCharLocalPos.push_back(inCharTrans.back() * glm::vec4(0,0,0,1));
    //     inCharLocalRot.push_back(glm::mat3(inCharTrans.back()));
    // }
    isOffGroundCheck(inCharLocalPos, gScene, charLocalToWorld);
}

void FootIK::blendingRatioUpdate(const std::chrono::steady_clock::time_point& curTime)
{
    std::chrono::milliseconds millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _callTime);
    if (_targetOn == false && _blendingRatio <= 0)
    {
        _callTime = curTime;
        _isSaveAnimation = false;
        return;
    }
    else if (_targetOn == false && _blendingRatio > 0)
        _blendingRatio -= static_cast<float>(millisecond.count()) / 200.0f;
    else if (_targetOn == true && _blendingRatio < 1.0f)
        _blendingRatio += static_cast<float>(millisecond.count()) / 200.0f;
    if (_blendingRatio >= 1.0f)
    {
        _blendingRatio = 1;
    } 
    else if (_blendingRatio < 0)
    {
        _blendingRatio = 0;
    }
    
    _callTime = curTime;
}
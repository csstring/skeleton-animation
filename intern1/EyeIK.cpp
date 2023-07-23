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
    if (reachable(characterTranspos, distance, headPosition) == true)//distance 구하는걸로 수정
    {
        glm::vec3 start = IKpos.front();
        uint32 iterCount = 0;
        while (glm::length(headPosition - IKpos.back()) > 0.1)
        {
            std::cout << "in IK" <<std::endl;
            iterCount++;
            if (iterCount >= 10)
            {
                std::cout << "no ik" << std::endl;
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
    glm::vec3 curSee = IKrot[_bonedirection.size()-2] * glm::vec4(_bonedirection[_bonedirection.size()-2],1);
    glm::quat afterSee = glm::rotation(glm::normalize(curSee), glm::normalize(IKpos[_bonedirection.size()-2] - IKpos[_bonedirection.size()-3]));
    IKrot[_bonedirection.size()-2] = glm::toMat4(afterSee) * IKrot[_bonedirection.size()-2];
    // IKrot[_bonedirection.size()-2] = glm::toMat4(glm::rotation(_boneVector[_eyeBoneIndex[_bonedirection.size()-2]]._direction, 
    // IKpos[_bonedirection.size()-2] - IKpos[_bonedirection.size()-3]));
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
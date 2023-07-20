#include "include/EyeIK.h"
#include "include/Bone.h"
#include <algorithm>
//parent bone 파싱 넣어야함
void EyeIK::setTargetPosition(glm::vec3 targetPosition)
{
    _targetPosition = targetPosition;
}

void EyeIK::initialize(const std::vector<Bone>& boneVector)
{
    const Bone* curBone = nullptr;
    for (const Bone& bone : boneVector)
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
        curBone = &boneVector[curBone->_parentBoneIndex];
        if (curBone->_boneIndex == BONEID::THORAX)
            break;
    }
    std::reverse(_eyeBoneIndex.begin(), _eyeBoneIndex.end());
    _IKTranspos.resize(boneVector.size(), glm::mat4(0.0f));
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

bool EyeIK::reachable(const std::vector<glm::mat4>& characterTranspos, std::vector<float>& distance)
{
    glm::vec3 start = characterTranspos[_eyeBoneIndex.front()] * glm::vec4(0,0,0,1);
    glm::vec3 end = characterTranspos[_eyeBoneIndex.back()] * glm::vec4(0,0,0,1);
    float targetDistance = glm::length(_targetPosition - start);
    float curDistance = 0;
    for (uint32 i = 1; i < _eyeBoneIndex.size(); ++i)
    {
        end = characterTranspos[_eyeBoneIndex[i]] * glm::vec4(0,0,0,1);
        distance.push_back(glm::length(start - end));
        curDistance += distance.back();
        start = end;
    }
    if (curDistance >= targetDistance)
        return true;
    else
        return false;
}

const std::vector<glm::mat4>& EyeIK::solveEyeIK(const std::vector<glm::mat4>& characterTranspos)
{
    std::vector<glm::vec3> IKpos;
    std::vector<glm::mat4> IKrot;
    std::vector<float> distance;
    for (auto& it : _IKTranspos)
        it = glm::mat4(0.0f);
    for (uint32 i : _eyeBoneIndex)
    {
        IKpos.push_back(characterTranspos[i] * glm::vec4(0,0,0,1));
        IKrot.push_back(glm::mat3(characterTranspos[i]));
    }
    //rotation
    /*
    start = glm::normalize(start);
    end = glm::normalize(end);
    glm::vec3 axis = glm::cross(start, end);
    glm::vec3 axisNormal = glm::normalize(axis);
    float angle = glm::acos(glm::dot(start, end));
    return glm::rotate(angle, axisNormal);
    */
    for (uint32 i = 0; i < _bonedirection.size(); ++i)
    {
        glm::vec3 start = IKrot[i] * glm::vec4(_bonedirection[i], 1);
        glm::vec3 end = _targetPosition - IKpos[i];
        glm::vec3 end1;
        start = glm::normalize(start);
        end = glm::normalize(end);
        glm::vec3 axis = glm::cross(end, start);
        end1 = glm::cross(axis, end);
        IKrot[i] = ft_rotate(start, end1) * IKrot[i];
    }
    //if reachable position
    // if (reachable(characterTranspos, distance) == true)
    // {
    //     glm::vec3 start = IKpos.front();
    //     while (glm::length(_targetPosition - IKpos.back()) > 0.0001)
    //     {
    //         IKpos.back() = _targetPosition;
    //         for(uint16 i = _eyeBoneIndex.size()-1; i >=1; --i)
    //         {
    //             float r = glm::length(IKpos[i] - IKpos[i-1]);
    //             float k = distance[i-1] / r;
    //             IKpos[i-1] = glm::mix(IKpos[i], IKpos[i-1], k);
    //         }

    //         IKpos.front() = start;
    //         for(uint16 i = 0; i < _eyeBoneIndex.size()-1; ++i)
    //         {
    //             float r = glm::length(IKpos[i+1] - IKpos[i]);
    //             float k = distance[i] / r;
    //             IKpos[i+1] = glm::mix(IKpos[i], IKpos[i+1], k);
    //         }
    //     }
    //     //위쪽에서 rot먼저해서 초기화 되면 glm::mat4(1.0f) 아니라 본인 넣어야함
    // }
    for (uint32 i = 0; i < _eyeBoneIndex.size(); ++i)
    {
        glm::mat4 translate = glm::translate(glm::mat4(1.0f), IKpos[i]);
        _IKTranspos[_eyeBoneIndex[i]] = translate * IKrot[i];
    }
    return _IKTranspos;
}
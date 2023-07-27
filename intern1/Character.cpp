#include "include/Character.h"
#include "include/Skeleton.h"
#include "include/TimeNode.h"
#include "include/Animation.h"
#include "include/GL/glew.h"
#include "include/Cylinder.h"
#include "include/GLM/gtx/compatibility.hpp"
#include "include/GLM/gtx/spline.hpp"
#include "include/GLM/gtx/quaternion.hpp"
#include "include/Controller.h"
#include "include/EnumHeader.h"
#include "include/EyeIK.h"
#include <queue>
inline bool pairCompare(const std::pair<uint32, glm::quat>& a, const uint32& val)
{
    return a.first < val;
}

void Character::rotationY(float radian)
{
    _worldRotation = glm::rotate(_worldRotation, radian, glm::vec3(0,1,0));
}

void Character::initialize(void)
{
    int64 size = _skeleton.getBoneVector().size();

    VAO.resize(size);
    VBO.resize(size);
    VBC.resize(size);

    _boneLocalVector.resize(size);
    _worldTrans = glm::mat4(1.0f);
    _worldRotation = glm::mat4(1.0f);
    boneBufferMaping();

    _eyeIK = new EyeIK(_skeleton.getBoneVector());
    _eyeIK->initialize();
}

void Character::boneBufferMaping(void)
{
    const std::vector<Bone>& boneVector = _skeleton.getBoneVector();
    uint32 boneIndex = 0;
    for(const Bone& bone : boneVector)
    {
        glGenVertexArrays(1, &VAO[bone._boneIndex]);
        glBindVertexArray(VAO[bone._boneIndex]);

        glGenBuffers(1, &VBO[bone._boneIndex]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[bone._boneIndex]);
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &VBC[bone._boneIndex]);
        glBindBuffer(GL_ARRAY_BUFFER, VBC[bone._boneIndex]);
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
}

void Character::getTransFormByKeyFrame(glm::quat& interpolR, glm::vec3& interpolT, const AnimationData* node, uint32 keyFrame)
{
    auto itR = std::lower_bound(node->_localRotation.begin(), node->_localRotation.end(), keyFrame, pairCompare);
    if (itR == node->_localRotation.end()) 
        itR--;
    if (itR + 1 != node->_localRotation.end())
        itR++;
    uint32 frontDistance = std::distance(node->_localRotation.begin(), itR);
    if (frontDistance < 3) itR = node->_localRotation.begin() + 3;
    if (keyFrame <= (itR -2)->first) keyFrame = (itR -2)->first + 1;
    else if (keyFrame > (itR - 1)->first) keyFrame = (itR - 1)->first;
    const glm::quat& t0 = (itR - 3)->second;
    const glm::quat& t1 = (itR - 2)->second;
    const glm::quat& t2 = (itR - 1)->second;
    const glm::quat& t3 = (itR - 0)->second;
    interpolR = glm::catmullRom(t0,t1,t2,t3, (keyFrame - (itR -2)->first)/((itR-1)->first - (itR -2)->first));

    auto itT = std::lower_bound(node->_localTrans.begin(), node->_localTrans.end(), keyFrame, pairCompare);
    itT++;
    const glm::vec3& v0 = (itT - 3)->second;
    const glm::vec3& v1 = (itT - 2)->second;
    const glm::vec3& v2 = (itT - 1)->second;
    const glm::vec3& v3 = (itT - 0)->second;
    interpolT = glm::catmullRom(v0,v1,v2,v3,(keyFrame - (itT -2)->first)/((itT-1)->first - (itT -2)->first));
}

void Character::updateTransForm(const AnimationData& node, uint32 keyFrame, float interpolVal)//fixme
{
    std::queue<const AnimationData*> animationQ;
    
    if (interpolVal > 1)
        interpolVal = 1;

    animationQ.push(&node);
    while (animationQ.empty() == false)
    {
        const AnimationData* curData = animationQ.front();
        animationQ.pop();
        for (const AnimationData& animation : curData->_childrens)
            animationQ.push(&animation);
        glm::quat mixR, originR;
        glm::vec3 mixT, originT;
        getTransFormByKeyFrame(mixR, mixT, curData, keyFrame);
        if (_isFirstBlend == true)
        {
            originR = glm::quat(0,0,0,1);
            originT = glm::vec3(0.0f);
        }
        else
        {
            originR = _boneLocalVector[curData->_boneIndex].rotationInBoneLocal;
            originT = _boneLocalVector[curData->_boneIndex].translationInBoneLocal;
        }
        _boneLocalVector[curData->_boneIndex].translationInBoneLocal = glm::mix(originT, mixT, interpolVal);
        _boneLocalVector[curData->_boneIndex].rotationInBoneLocal = glm::slerp(originR, mixR, interpolVal);
    }
    _isFirstBlend = false;
}

void Character::eraseAnimation(const std::chrono::steady_clock::time_point& curTime)
{
    if (_lowerBodyAnimation.empty() == false && curTime >= _lowerBodyAnimation.begin()->second._endTime)
        _lowerBodyAnimation.pop_front();
    if (_upperBodyAnimation.empty() == false && curTime >= _upperBodyAnimation.begin()->second._endTime)
        _upperBodyAnimation.pop_front();

    if (_baseAnimation.size() == 1 && 
            getAfterTimePoint(OVERLAPTIME) >= _baseAnimation.begin()->second._endTime)
    {
        const Animation* pushAnimation = _baseAnimation.front().first;
        TimeNode node(getCurTimePoint(), getAfterTimePoint(pushAnimation->_animationMillisecond));
        _baseAnimation.push_back({pushAnimation, node});
    }
    if (curTime >= _baseAnimation.begin()->second._endTime)
    {
        _baseAnimation.pop_front();
    }
}


void Character::worldPositionUpdate(void)
{
    glm::vec3 t = _worldTrans * _worldRotation * _controller.getMatrixInCharLocal(0, _skeleton, _boneLocalVector) * glm::vec4(0,0,0,1);
    _worldTrans = glm::translate(glm::mat4(1.0f), t);
}

void Character::update(const std::chrono::steady_clock::time_point& curTime, glm::vec3 eyeTarget)
{
    eraseAnimation(curTime);
    worldPositionUpdate();
    _isFirstBlend = true;

    std::chrono::milliseconds  millisecondFromBegin, millisecondToEnd;
    uint64 keyFrame;
    const Animation* animation;
    float interpolVal;
    //lower
    if (_baseAnimation.empty() == false)
    {
        animation = _baseAnimation.begin()->first;
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _baseAnimation.begin()->second._startTime);
        updateTransForm(animation->_rootNode, millisecondFromBegin.count()*120/1000, 1);
    }
    if (_baseAnimation.size() >= 2)
    {
        animation = _baseAnimation[1].first;
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _baseAnimation[1].second._startTime);
        interpolVal = static_cast<float>(millisecondFromBegin.count()) / OVERLAPTIME;

        updateTransForm(animation->_rootNode, millisecondFromBegin.count()*120/1000, interpolVal);
    }
    if (_lowerBodyAnimation.empty() == false) 
    {
        animation = _lowerBodyAnimation.front().first;
        millisecondToEnd = std::chrono::duration_cast<std::chrono::milliseconds>(_lowerBodyAnimation.front().second._endTime - curTime);
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _lowerBodyAnimation.front().second._startTime);
        if (millisecondToEnd.count() <= OVERLAPTIME)
            interpolVal = static_cast<float>(millisecondToEnd.count()) / OVERLAPTIME;
        else
            interpolVal = static_cast<float>(millisecondFromBegin.count()) / OVERLAPTIME;
        
        updateTransForm(animation->_rootNode, millisecondFromBegin.count()*120/1000, interpolVal);
    }
    if (_upperBodyAnimation.empty() == false) 
    {
        animation = _upperBodyAnimation.front().first;
        millisecondToEnd = std::chrono::duration_cast<std::chrono::milliseconds>(_upperBodyAnimation.front().second._endTime - curTime);
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _upperBodyAnimation.front().second._startTime);
        if (millisecondToEnd.count() <= OVERLAPTIME)
            interpolVal = static_cast<float>(millisecondToEnd.count()) / OVERLAPTIME;
        else
            interpolVal = static_cast<float>(millisecondFromBegin.count()) / OVERLAPTIME;

        updateTransForm(*animation->returnAnimationData(11/*lowerback*/), millisecondFromBegin.count()*120/1000, interpolVal);
    }
    _eyeIK->setTargetPosition(eyeTarget);

    {
        _eyeIK->solveEyeIK(_boneLocalVector, _worldRotation, _worldTrans, _controller);
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _eyeIK->_start);
        // animationBlending(millisecondFromBegin ,IKTrans);
    }
}

//up -> draw
void Character::draw(void)
{
    const std::vector<Bone>& boneVector = _skeleton.getBoneVector();

    glm::vec3 color(0.9412, 0.7922, 0.2549);
    for(const Bone& bone : boneVector)
    {
        glBindVertexArray(VAO[bone._boneIndex]);
       glm::mat4 toParentDir = _worldTrans * _worldRotation * _controller.getMatrixInCharLocal(bone._boneIndex, _skeleton, _boneLocalVector) * ft_rotate(glm::vec3(0.0,0.0,1.0), -bone._direction);// * glm::inverse(test3);
        Cylinder cylinder(0.2, 1.0 *_skeleton.getGBL() * bone._length ,16, toParentDir);
        cylinder.initialize(color, VBC[bone._boneIndex], static_cast<BONEID>(bone._boneIndex));
        cylinder.render(VBO[bone._boneIndex]);
        // Line line(0.7 *_skeleton.getGBL() * bone._length, toParentDir);
        // line.initialize(color, VBC[bone._boneIndex]);
        // line.render(VBO[bone._boneIndex]);
        glBindVertexArray(0);
    }
}





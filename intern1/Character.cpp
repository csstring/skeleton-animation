#include "include/Character.h"
#include "include/Skeleton.h"
#include "include/TimeNode.h"
#include "include/Animation.h"
#include "include/GL/glew.h"
#include "include/Cylinder.h"
#include "include/GLM/gtx/spline.hpp"
#include "include/GLM/gtx/quaternion.hpp"

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

    _worldTrans = glm::mat4(1.0f);
    _worldRotation = glm::mat4(1.0f);
    _transForm.resize(size);
    _finalRot.resize(size);
    _backTransForm.resize(size);
    _upperTransForm.resize(size, glm::vec4(0.0f));
    boneBufferMaping();
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

void Character::updateTransForm(const AnimationData& node, glm::mat4 wolrdTrans, uint32 keyFrame, TransFormFix fix)//fixme
{
    auto itR = std::lower_bound(node._localRotation.begin(), node._localRotation.end(), keyFrame, pairCompare);
    if (itR == node._localRotation.end()) 
        itR--;
    if (itR + 1 != node._localRotation.end())
        itR++;
    uint32 frontDistance = std::distance(node._localRotation.begin(), itR);
    if (frontDistance < 3) itR = node._localRotation.begin() + 3;
    if (keyFrame <= (itR -2)->first) keyFrame = (itR -2)->first + 1;
    else if (keyFrame > (itR - 1)->first) keyFrame = (itR - 1)->first;
    const glm::quat& t0 = (itR - 3)->second;
    const glm::quat& t1 = (itR - 2)->second;
    const glm::quat& t2 = (itR - 1)->second;
    const glm::quat& t3 = (itR - 0)->second;
    glm::quat interpolR = glm::catmullRom(t0,t1,t2,t3, (keyFrame - (itR -2)->first)/((itR-1)->first - (itR -2)->first));

    auto itT = std::lower_bound(node._localTrans.begin(), node._localTrans.end(), keyFrame, pairCompare);
    itT++;
    const glm::mat4& v0 = (itT - 3)->second;
    const glm::mat4& v1 = (itT - 2)->second;
    const glm::mat4& v2 = (itT - 1)->second;
    const glm::mat4& v3 = (itT - 0)->second;
    glm::mat4 interpolT = glm::catmullRom(v0,v1,v2,v3,(keyFrame - (itT -2)->first)/((itT-1)->first - (itT -2)->first));

    wolrdTrans = wolrdTrans * interpolT * glm::toMat4(interpolR);
    if (fix == TransFormFix::LOWERFRONT)
    {
        _transForm[node._boneIndex] = wolrdTrans * glm::vec4(0,0,0,1);
        _finalRot[node._boneIndex] = (glm::mat3)wolrdTrans;
    }
    else if (fix == TransFormFix::LOWERBACK)
        _backTransForm[node._boneIndex] = wolrdTrans * glm::vec4(0,0,0,1);
    else if (fix == TransFormFix::UPPERFRONT)
        _upperTransForm[node._boneIndex] = wolrdTrans * glm::vec4(0,0,0,1);
    for (const AnimationData& child : node._childrens)
        updateTransForm(child, wolrdTrans, keyFrame, fix);
}

void Character::eraseAnimation(const std::chrono::steady_clock::time_point& curTime)
{
    if (_lowerBodyBackAnimation.empty() == false && curTime >= _lowerBodyBackAnimation.begin()->second._endTime)
        _lowerBodyBackAnimation.pop_front();
    if (_upperBodyAnimation.empty() == false && curTime >= _upperBodyAnimation.begin()->second._endTime)
        _upperBodyAnimation.pop_front();

    if (_lowerBodyAnimation.size() == 1 && 
            getAfterTimePoint(OVERLAPTIME) >= _lowerBodyAnimation.begin()->second._endTime)
    {
        Animation* pushAnimation = _lowerBodyAnimation.front().first;
        TimeNode node(getCurTimePoint(), getAfterTimePoint(pushAnimation->_animationMillisecond));
        _lowerBodyAnimation.push_back({pushAnimation, node});
    }
    if (curTime >= _lowerBodyAnimation.begin()->second._endTime)
    {
        _lowerBodyAnimation.pop_front();
    }
}

void Character::animationBlending(const std::chrono::milliseconds& time, const std::vector<glm::vec4>& mixTrans)
{
    float interpolVal = static_cast<float>(time.count()) / OVERLAPTIME;
    if (interpolVal > 1)
        interpolVal = 1;
    for (uint8 i = 0; i < _transForm.size(); ++i)
    {
        if (mixTrans[i] == glm::vec4(0.0f))
            continue;
        _transForm[i] = glm::mix(_transForm[i], mixTrans[i], interpolVal);
    }
}

void Character::update(const std::chrono::steady_clock::time_point& curTime)
{
    eraseAnimation(curTime);
    
    std::chrono::milliseconds millisecondFromBegin, millisecondToEnd;
    uint64 keyFrame;
    Animation* animation;
    //lower
    if (_lowerBodyAnimation.empty() == false)
    {
        animation = _lowerBodyAnimation.begin()->first;
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _lowerBodyAnimation.begin()->second._startTime);
        updateTransForm(animation->_rootNode, _worldTrans*_worldRotation, millisecondFromBegin.count()*120/1000, TransFormFix::LOWERFRONT);
    }
    if (_lowerBodyAnimation.size() >= 2)
    {
        animation = _lowerBodyAnimation[1].first;
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _lowerBodyAnimation[1].second._startTime);
        updateTransForm(animation->_rootNode, _worldTrans*_worldRotation, millisecondFromBegin.count()*120/1000, TransFormFix::LOWERBACK);

        animationBlending(millisecondFromBegin, _backTransForm);
    }
    if (_lowerBodyBackAnimation.empty() == false) 
    {
        animation = _lowerBodyBackAnimation.front().first;
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _lowerBodyBackAnimation.front().second._startTime);
        updateTransForm(animation->_rootNode, _worldTrans*_worldRotation, millisecondFromBegin.count()*120/1000, TransFormFix::LOWERBACK);
        millisecondToEnd = std::chrono::duration_cast<std::chrono::milliseconds>(_lowerBodyBackAnimation.front().second._endTime - curTime);
        if (millisecondToEnd.count() <= OVERLAPTIME)
            animationBlending(millisecondToEnd,_backTransForm);
        else
            animationBlending(millisecondFromBegin,_backTransForm);
    }
    if (_upperBodyAnimation.empty() == false) 
    {
        animation = _upperBodyAnimation.front().first;
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _upperBodyAnimation.front().second._startTime);
        updateTransForm(*animation->returnAnimationData(11/*lowerback*/), _worldTrans*_worldRotation, millisecondFromBegin.count()*120/1000, TransFormFix::UPPERFRONT);
        millisecondToEnd = std::chrono::duration_cast<std::chrono::milliseconds>(_upperBodyAnimation.front().second._endTime - curTime);
        if (millisecondToEnd.count() <= OVERLAPTIME)
            animationBlending(millisecondToEnd, _upperTransForm);
        else
            animationBlending(millisecondFromBegin, _upperTransForm);
    }
}

//up -> draw
void Character::draw(void)
{
    const std::vector<Bone>& boneVector = _skeleton.getBoneVector();

    glm::vec3 color(1.0f);
    for(const Bone& bone : boneVector)
    {
        glBindVertexArray(VAO[bone._boneIndex]);
        glm::vec3 dir = _transForm[bone._parentIndex] - _transForm[bone._boneIndex];
        glm::mat4 boneDir = _finalRot[bone._boneIndex] * ft_rotate(glm::vec3(0.0,0.0,1.0), bone._direction);// * glm::inverse(test3); 
        Cylinder cylinder(0.2, 0.8 *_skeleton.getGBL() * bone._length ,16, boneDir, _transForm[bone._boneIndex]);
        cylinder.initialize(color, VBC[bone._boneIndex], static_cast<BONEID>(bone._boneIndex));
        cylinder.render(VBO[bone._boneIndex]);
        // Line line(0.7 *_skeleton.getGBL() * bone._length, toParentDir);
        // line.initialize(color, VBC[bone._boneIndex]);
        // line.render(VBO[bone._boneIndex]);
        glBindVertexArray(0);
    }
    //아래쪽 업데이트 인거 같은데
    _worldTrans = glm::translate(glm::mat4(1.0f), (glm::vec3)_transForm[0]);
}
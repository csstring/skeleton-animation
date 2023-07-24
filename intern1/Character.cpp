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
    _baseTransForm.resize(size);
    _lowerTransForm.resize(size);
    _upperTransForm.resize(size, glm::mat4(0.0f));
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
        _baseTransForm[node._boneIndex] = wolrdTrans;
        // if (node._boneIndex == 0)
        // {
        //     glm::quat rot(wolrdTrans);
        //     static int i = 0;
        //     static glm::vec4 trans(0.0f);
        //     trans += interpolT * glm::vec4(0,0,0,1);
        //     glm::vec3 euler =  glm::eulerAngles(rot);
        //     std::cout << glm::to_string(trans) << " index : " << i << "key : " << keyFrame<< std::endl;
        //     i++;
        // }
    }
    else if (fix == TransFormFix::LOWERBACK)
        _lowerTransForm[node._boneIndex] = wolrdTrans;
    else if (fix == TransFormFix::UPPERFRONT)
        _upperTransForm[node._boneIndex] = wolrdTrans;
    for (const AnimationData& child : node._childrens)
        updateTransForm(child, wolrdTrans, keyFrame, fix);
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
        Animation* pushAnimation = _baseAnimation.front().first;
        TimeNode node(getCurTimePoint(), getAfterTimePoint(pushAnimation->_animationMillisecond));
        _baseAnimation.push_back({pushAnimation, node});
    }
    if (curTime >= _baseAnimation.begin()->second._endTime)
    {
        _baseAnimation.pop_front();
    }
}

void Character::animationBlending(const std::chrono::milliseconds& time, const std::vector<glm::mat4>& mixTrans)
{
    float interpolVal = static_cast<float>(time.count()) / OVERLAPTIME;
    if (interpolVal > 1)
        interpolVal = 1;
    for (uint8 i = 0; i < _baseTransForm.size(); ++i)
    {
        if (mixTrans[i] == glm::mat4(0.0f))
            continue;
        glm::quat rotBase(_baseTransForm[i]);
        glm::quat rotMix(mixTrans[i]);
        glm::vec4 translateBase = _baseTransForm[i] * glm::vec4(0,0,0,1);
        glm::vec4 translateMix = mixTrans[i] * glm::vec4(0,0,0,1);
        glm::quat rot = glm::slerp(rotBase, rotMix, interpolVal);
        glm::vec3 trans = glm::mix(translateBase, translateMix, interpolVal);
        _baseTransForm[i] = glm::translate(glm::mat4(1.0f), trans) * glm::toMat4(rot);
    }
}

void Character::update(const std::chrono::steady_clock::time_point& curTime, glm::vec3 eyeTarget)
{
    eraseAnimation(curTime);
    
    std::chrono::milliseconds millisecondFromBegin, millisecondToEnd;
    uint64 keyFrame;
    Animation* animation;
    //lower
    if (_baseAnimation.empty() == false)
    {
        animation = _baseAnimation.begin()->first;
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _baseAnimation.begin()->second._startTime);
        updateTransForm(animation->_rootNode, _worldTrans*_worldRotation, millisecondFromBegin.count()*120/1000, TransFormFix::LOWERFRONT);
    }
    if (_baseAnimation.size() >= 2)
    {
        animation = _baseAnimation[1].first;
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _baseAnimation[1].second._startTime);
        updateTransForm(animation->_rootNode, _worldTrans*_worldRotation, millisecondFromBegin.count()*120/1000, TransFormFix::LOWERBACK);

        animationBlending(millisecondFromBegin, _lowerTransForm);
    }
    if (_lowerBodyAnimation.empty() == false) 
    {
        animation = _lowerBodyAnimation.front().first;
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _lowerBodyAnimation.front().second._startTime);
        updateTransForm(animation->_rootNode, _worldTrans*_worldRotation, millisecondFromBegin.count()*120/1000, TransFormFix::LOWERBACK);
        millisecondToEnd = std::chrono::duration_cast<std::chrono::milliseconds>(_lowerBodyAnimation.front().second._endTime - curTime);
        if (millisecondToEnd.count() <= OVERLAPTIME)
            animationBlending(millisecondToEnd,_lowerTransForm);
        else
            animationBlending(millisecondFromBegin,_lowerTransForm);
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
    //world position update
    glm::vec4 end1 = _baseTransForm[0] * glm::vec4(0,0,0,1);
    _worldTrans = glm::translate(glm::mat4(1.0f), (glm::vec3)end1);

    // _eyeIK->setTargetPosition(eyeTarget);
    // if (_eyeIK->targetPositionCheck(_baseTransForm))
    // {
    //     const std::vector<glm::mat4>& IKTrans = _eyeIK->solveEyeIK(_baseTransForm, _worldRotation);
    //     millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _eyeIK->_start);
    //     animationBlending(millisecondFromBegin ,IKTrans);
    // }
}

//up -> draw
void Character::draw(void)
{
    const std::vector<Bone>& boneVector = _skeleton.getBoneVector();

    glm::vec3 color(0.9412, 0.7922, 0.2549);
    for(const Bone& bone : boneVector)
    {
        glBindVertexArray(VAO[bone._boneIndex]);
        // if (bone._boneIndex == 0)
        // {
        //     glm::quat rot(_baseTransForm[0]);
        //     glm::vec4 trans = _baseTransForm[0] * glm::vec4(0,0,0,1);
        //     glm::vec3 euler =  glm::eulerAngles(rot);
        //     std::cout << glm::to_string(trans) << std::endl;
        // }
        glm::mat4 toParentDir = _baseTransForm[bone._boneIndex] * ft_rotate(glm::vec3(0.0,0.0,1.0), -bone._direction);// * glm::inverse(test3); 
        Cylinder cylinder(0.2, 1.0 *_skeleton.getGBL() * bone._length ,16, toParentDir);
        cylinder.initialize(color, VBC[bone._boneIndex], static_cast<BONEID>(bone._boneIndex));
        cylinder.render(VBO[bone._boneIndex]);
        // Line line(0.7 *_skeleton.getGBL() * bone._length, toParentDir);
        // line.initialize(color, VBC[bone._boneIndex]);
        // line.render(VBO[bone._boneIndex]);
        glBindVertexArray(0);
    }
}
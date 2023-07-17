#include "include/Simulator.h"
#include "include/Animation.h"
#include "include/Skeleton.h"
#include "include/GL/glew.h"
#include "include/Cylinder.h"
#include "include/AnimationCompressor.h"
#include "include/GLM/gtx/spline.hpp"
#include "include/GLM/gtx/quaternion.hpp"
#include "include/Line.h"
#include <algorithm>
#include <queue>
const float compressMul[] = {0 ,10.5, 94.6615, 355.184};
void Simulator::initialize(void)
{
    int64 size = _skeleton.getBoneVector().size();

    VAO.resize(size);
    VBO.resize(size);
    VBC.resize(size);

    _worldTrans = glm::mat4(1.0f);
    _worldRotation = glm::mat4(1.0f);
    _worldRotBuffer = glm::mat4(1.0f);
    _transForm.resize(size);
    _backTransForm.resize(size);
    Animation* pushAnimation = findAnimation("idle");//find
    TimeNode node(getCurTimePoint(), getAfterTimePoint(pushAnimation->_animationMillisecond));

    _lowerBodyAnimation.push_back({pushAnimation, node});
    //_upperBodyAnimation.push_back({pushAnimation, node});
    boneBufferMaping();
}

void Simulator::boneBufferMaping(void)
{
    std::vector<Bone>& boneVector = _skeleton.getBoneVector();
    uint32 boneIndex = 0;
    for(Bone& bone : boneVector)
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

void Simulator::draw(void)
{
    std::vector<Bone>& boneVector = _skeleton.getBoneVector();

    for (int i =0; i <=0; ++i)//comprees test
    {
        update();
        glm::vec3 color(0.0f);
        color[i] = 1;
        for(Bone& bone : boneVector)
        {
            glBindVertexArray(VAO[bone._boneIndex]);
            glm::mat4 toParentDir = _worldTrans * _transForm[bone._boneIndex] * ft_rotate(glm::vec3(0.0,0.0,1.0), bone._direction);// * glm::inverse(test3); 
            Cylinder cylinder(0.2, 0.7 *_skeleton.getGBL() * bone._length ,16, toParentDir);
            cylinder.initialize(color, VBC[bone._boneIndex]);
            cylinder.render(VBO[bone._boneIndex]);
            // Line line(0.7 *_skeleton.getGBL() * bone._length, toParentDir);
            // line.initialize(color, VBC[bone._boneIndex]);
            // line.render(VBO[bone._boneIndex]);
            glBindVertexArray(0);
        }
        glm::vec4 end1 = _transForm[0] * glm::vec4(0,0,0,1);
        end1.y = 0;
        _worldTrans = glm::translate(_worldTrans, (glm::vec3)end1);
    }
}

inline bool pairCompare(const std::pair<uint32, glm::quat>& a, const uint32& val)
{
    return a.first < val;
}

void Simulator::getFrameIterator(uint32* keyArray, uint32& findKeyFrame, const std::vector<std::pair<uint32,glm::quat>>& animationFrame)//time? index?
{
    auto it = std::lower_bound(animationFrame.begin(), animationFrame.end(), findKeyFrame, pairCompare);
    if (it != animationFrame.end() && it + 1 != animationFrame.end())
        it++;
    uint32 frontDistance = std::distance(animationFrame.begin(), it);
    if (frontDistance < 3) it = animationFrame.begin() + 3;
}

void Simulator::updateTransForm(const AnimationData& node, glm::mat4 wolrdTrans, uint32 keyFrame, TransFormFix fix)//fixme
{
    auto itR = std::lower_bound(node._localRotation.begin(), node._localRotation.end(), keyFrame, pairCompare);
    if (itR == node._localRotation.end()) 
        itR--;
    if (itR + 1 != node._localRotation.end())
        itR++;
    uint32 frontDistance = std::distance(node._localRotation.begin(), itR);
    if (frontDistance < 3) itR = node._localRotation.begin() + 3;
    if (keyFrame < (itR -2)->first) keyFrame = (itR -2)->first + 1;
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
    if (fix == TransFormFix::FRONT)
    {
        _transForm[node._boneIndex] = wolrdTrans;
    }
    else if (fix == TransFormFix::BACK)
        _backTransForm[node._boneIndex] = wolrdTrans;

    for (const AnimationData& child : node._childrens)
        updateTransForm(child, wolrdTrans, keyFrame, fix);
}

void Simulator::eraseAnimation(std::chrono::steady_clock::time_point& curTime)
{
    // if (curTime >= _upperBodyAnimation.begin()->second)
    // {
    //     _upperBodyAnimation.pop_front();
    //     if (_upperBodyAnimation.empty())//일단 walk fixme
    //     {
    //         Animation* pushAnimation = findAnimation("walk");//find
    //         _upperBodyAnimation.push_back({pushAnimation, getAfterTimePoint(pushAnimation->_animationMillisecond)});
    //     }
    // }
    if (_lowerBodyBackAnimation.empty() == false && curTime >= _lowerBodyBackAnimation.begin()->second._endTime)
        _lowerBodyBackAnimation.pop_front();

    if (_lowerBodyAnimation.size() == 1 && 
            getAfterTimePoint(OVERLAPTIME) >= _lowerBodyAnimation.begin()->second._endTime)
    {
        Animation* pushAnimation = _lowerBodyAnimation.front().first;
        TimeNode node(getCurTimePoint(), getAfterTimePoint(pushAnimation->_animationMillisecond));
        _lowerBodyAnimation.push_back({pushAnimation, node});
    }
    if (curTime >= _lowerBodyAnimation.begin()->second._endTime)
        _lowerBodyAnimation.pop_front();
}

void Simulator::animationBlending(const std::chrono::milliseconds& time)
{
    float interpolVal = static_cast<float>(time.count()) / OVERLAPTIME;
    if (interpolVal > 1)
    {
        for (auto& it : _lowerBodyAnimation){
            std::cout << it;
        }
        return;
    }
    for (uint8 i = 0; i < _transForm.size(); ++i)
    {
        _transForm[i] = glm::mix(_transForm[i], _backTransForm[i], interpolVal);
    }
}

void Simulator::update()
{
    std::chrono::steady_clock::time_point curTime = getCurTimePoint();
    eraseAnimation(curTime);
    
    std::chrono::milliseconds curMillisecond;
    uint64 keyFrame;
    Animation* animation;
    //lower
    if (_lowerBodyAnimation.empty() == false)
    {
        animation = _lowerBodyAnimation.begin()->first;
        curMillisecond = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _lowerBodyAnimation.begin()->second._startTime);
        updateTransForm(animation->_rootNode, _worldRotation, curMillisecond.count()*120/1000, TransFormFix::FRONT);
    }
    if (_lowerBodyAnimation.size() >= 2)
    {
        for (auto& it : _lowerBodyAnimation){
            if (curTime > it.second._endTime)
                ft_assert("time");
        }
        animation = _lowerBodyAnimation[1].first;
        curMillisecond = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _lowerBodyAnimation[1].second._startTime);
        updateTransForm(animation->_rootNode, _worldRotation, curMillisecond.count()*120/1000, TransFormFix::BACK);

        animationBlending(curMillisecond);
    } else if (_lowerBodyBackAnimation.empty() == false) {
        animation = _lowerBodyBackAnimation.front().first;
        curMillisecond = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _lowerBodyBackAnimation.front().second._startTime);
        updateTransForm(animation->_rootNode, _worldRotation, curMillisecond.count()*120/1000, TransFormFix::BACK);

        animationBlending(curMillisecond);
    }
    //upper
    // if (_upperBodyAnimation.empty() == false)
    // {
    //     animation = _upperBodyAnimation.begin()->first;
    //     curMillisecond = std::chrono::duration_cast<std::chrono::milliseconds>(_upperBodyAnimation.begin()->second - curTime);
    //     updateTransForm(*animation->returnAnimationData(11/*lowerback*/), _transForm[0], (animation->_animationMillisecond - curMillisecond.count())*120/1000);
    // }
    //보간

}

Animation* Simulator::findAnimation(const std::string& name)
{
    for (int32 i = 0; i < _animations.size(); ++i)
    {
        if (_animations[i]._name == name)
            return &_animations[i];
    }
    return NULL;
}

void Simulator::pushAnimation(Animation* pushAnimation, std::deque<std::pair<Animation*, TimeNode>>& animationDeque)
{
    if (animationDeque.size() >= 3)
        animationDeque.pop_back();
    if (animationDeque.size() >= 1 &&
        animationDeque.back().second._endTime > getAfterTimePoint(OVERLAPTIME))
        animationDeque.back().second._endTime = getAfterTimePoint(OVERLAPTIME);

    TimeNode node(getCurTimePoint(), getAfterTimePoint(pushAnimation->_animationMillisecond));
    animationDeque.push_back({pushAnimation, node});
}

void Simulator::changeAnimation(KeyInput key)//rot도 보간을 해야하나 일단 돌려놓기
{
    if (key == KeyInput::UP)
    {
        Animation* pushAnimation = findAnimation("walk");//find
        this->pushAnimation(pushAnimation, _lowerBodyAnimation);
    }
    else if (key == KeyInput::BACK)
    {
        _worldRotation = glm::rotate(_worldRotation, PI, glm::vec3(0,1,0));//애니메이션 끝나고 돌려야함
    }
    else if (key == KeyInput::REFT)
    {
        _worldRotation = glm::rotate(_worldRotation, PI/(2*30), glm::vec3(0,1,0));
    }
    else if (key == KeyInput::RIGHT)
    {
        _worldRotation = glm::rotate(_worldRotation, -PI/(2*30), glm::vec3(0,1,0));
    }
    else if (key == KeyInput::STOP)
    {
        std::cout << "idle input" << std::endl;
        Animation* pushAnimation = findAnimation("idle");//find
        this->pushAnimation(pushAnimation, _lowerBodyAnimation);
    }
    else if (key == KeyInput::RUN)
    {
        if (_lowerBodyAnimation.begin()->first->_name == "walk")
        {
            Animation* pushAnimation = findAnimation("run");//find
            this->pushAnimation(pushAnimation, _lowerBodyAnimation);
        }
        else if (_lowerBodyAnimation.begin()->first->_name == "run")
        {
            Animation* pushAnimation = findAnimation("walk");//find
            this->pushAnimation(pushAnimation, _lowerBodyAnimation);
        }
    }
    else if (key == KeyInput::ATTACK && _upperBodyAnimation.size() < 2)
    {
        Animation* pushAnimation = findAnimation("attck");//find
        this->pushAnimation(pushAnimation, _upperBodyAnimation);
    }
    else if (key == KeyInput::JUMP && _lowerBodyAnimation.size() < 2)
    {
        Animation* pushAnimation = findAnimation("jump");//find
        if (_lowerBodyAnimation.front().first->_name == "idle")
            this->pushAnimation(pushAnimation, _lowerBodyAnimation);
        else
            this->pushAnimation(pushAnimation, _lowerBodyBackAnimation);
    }
}

std::ostream& operator<<(std::ostream& os, const std::pair<Animation*, TimeNode>& ref)
{
    std::cout << "name : " << ref.first->_name << "\n";
    std::cout << "millisecond : " << ref.first->_animationMillisecond << std::endl;
    return os;
}
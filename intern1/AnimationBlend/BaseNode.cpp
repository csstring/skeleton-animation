#include "../include/Animation.h"
#include "../include/AnimationBlend/BaseNode.h"

void BaseNode::eraseAnimation(const std::chrono::steady_clock::time_point& curTime)
{
    if (_animations.size() == 1 && 
            getAfterTimePoint(OVERLAPTIME) >= _animations.begin()->second._endTime)
    {
        const Animation* pushAnimation = _animations.front().first;
        TimeNode node(getCurTimePoint(), getAfterTimePoint(pushAnimation->_animationMillisecond));
        _animations.push_back({pushAnimation, node});
    }
    if (curTime >= _animations.begin()->second._endTime)
    {
        _animations.pop_front();
    }
}

void BaseNode::update(
    const std::chrono::steady_clock::time_point& curTime, 
    std::vector<BoneLocal>& _boneLocalVector,
    LowerState& lowerState,
    UpperState& upperState
)
{
    const Animation* animation;
    float interpolVal;
    std::chrono::milliseconds  millisecondFromBegin;

    if (_animations.empty() == false)//first interval =1;
    {
        animation = _animations.begin()->first;
        changeUpperState(upperState, animation->_name);
        changeLowerState(lowerState, animation->_name);
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _animations.begin()->second._startTime);
        updateTransForm(animation->_rootNode, millisecondFromBegin.count()*120/1000, 1, _boneLocalVector, {}, BlendNode::BASE);
    }
    if (_animations.size() >= 2)
    {
        animation = _animations[1].first;
        changeUpperState(upperState, animation->_name);
        changeLowerState(lowerState, animation->_name);//fixme
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _animations[1].second._startTime);
        interpolVal = static_cast<float>(millisecondFromBegin.count()) / OVERLAPTIME;

        updateTransForm(animation->_rootNode, millisecondFromBegin.count()*120/1000, interpolVal, _boneLocalVector, {}, BlendNode::BASE);
    }
}
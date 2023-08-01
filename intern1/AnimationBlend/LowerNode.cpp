#include "../include/Animation.h"
#include "../include/AnimationBlend/LowerNode.h"

void LowerNode::eraseAnimation(const std::chrono::steady_clock::time_point& curTime)
{
    if (_animations.empty() == false && curTime >= _animations.begin()->second._endTime)
        _animations.pop_front();
}

void LowerNode::update(
    const std::chrono::steady_clock::time_point& curTime, 
    std::vector<BoneLocal>& _boneLocalVector,
    LowerState& lowerState,
    UpperState& upperState
)
{
    const Animation* animation;
    float interpolVal;
    std::chrono::milliseconds  millisecondFromBegin, millisecondToEnd;

    if (_animations.empty() == false) 
    {
        animation = _animations.front().first;
        changeLowerState(lowerState, animation->_name);
        millisecondToEnd = std::chrono::duration_cast<std::chrono::milliseconds>(_animations.front().second._endTime - curTime);
        millisecondFromBegin = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _animations.front().second._startTime);
        if (millisecondToEnd.count() <= OVERLAPTIME)
            interpolVal = static_cast<float>(millisecondToEnd.count()) / OVERLAPTIME;
        else
            interpolVal = static_cast<float>(millisecondFromBegin.count()) / OVERLAPTIME;
    
        updateTransForm(animation->_rootNode, millisecondFromBegin.count()*120/1000, interpolVal, _boneLocalVector, {BONEID::LOWERBACK}, BlendNode::LOWER);
    }
}
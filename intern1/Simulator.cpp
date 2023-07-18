#include "include/Simulator.h"
#include "include/Animation.h"
#include "include/TimeNode.h"
#include "include/Character.h"

const float compressMul[] = {0 ,10.5, 94.6615, 355.184};
void Simulator::setPlayer(Character* player)
{
    if (_player != nullptr)
        delete _player;
    _player = player;
}

void Simulator::initialize(void)
{
    _player->initialize();

    Animation* pushAnimation = findAnimation("idle");//find
    TimeNode node(getCurTimePoint(), getAfterTimePoint(pushAnimation->_animationMillisecond));
    _player->_lowerBodyAnimation.push_back({pushAnimation, node});
}

void Simulator::draw(void)
{
    _player->draw();
}

void Simulator::update()
{
    std::chrono::steady_clock::time_point curTime = getCurTimePoint();
    _player->update(curTime);
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

void Simulator::changeAnimation(KeyInput key)
{
    if (key == KeyInput::UP)
    {
        Animation* pushAnimation = findAnimation("walk");//find
        this->pushAnimation(pushAnimation, _player->_lowerBodyAnimation);
    }
    else if (key == KeyInput::LOWERBACK)
    {
        _player->rotationY(PI);//애니메이션 끝나고 돌려야함
    }
    else if (key == KeyInput::REFT)
    {
        _player->rotationY(PI/(2*30));
    }
    else if (key == KeyInput::RIGHT)
    {
        _player->rotationY(-PI/(2*30));
    }
    else if (key == KeyInput::STOP)
    {
        std::cout << "idle input" << std::endl;
        Animation* pushAnimation = findAnimation("idle");//find
        this->pushAnimation(pushAnimation, _player->_lowerBodyAnimation);
    }
    else if (key == KeyInput::RUN)
    {
        if (_player->_lowerBodyAnimation.begin()->first->_name == "walk")
        {
            Animation* pushAnimation = findAnimation("run");//find
            this->pushAnimation(pushAnimation, _player->_lowerBodyAnimation);
        }
        else if (_player->_lowerBodyAnimation.begin()->first->_name == "run")
        {
            Animation* pushAnimation = findAnimation("walk");//find
            this->pushAnimation(pushAnimation, _player->_lowerBodyAnimation);
        }
    }
    else if (key == KeyInput::ATTACK && _player->_upperBodyAnimation.empty() == true)
    {
        Animation* pushAnimation = findAnimation("punch");//find
        this->pushAnimation(pushAnimation, _player->_upperBodyAnimation);
    }
    else if (key == KeyInput::JUMP && _player->_lowerBodyBackAnimation.empty() == true)
    {
        Animation* pushAnimation = findAnimation("runJump2");//find
        this->pushAnimation(pushAnimation, _player->_lowerBodyBackAnimation);
    }
}

std::ostream& operator<<(std::ostream& os, const std::pair<Animation*, TimeNode>& ref)
{
    std::cout << "name : " << ref.first->_name << "\n";
    std::cout << "millisecond : " << ref.first->_animationMillisecond << std::endl;
    return os;
}
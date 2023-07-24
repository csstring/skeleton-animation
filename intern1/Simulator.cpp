#include "include/Simulator.h"
#include "include/Animation.h"
#include "include/TimeNode.h"
#include "include/Character.h"
#include "include/Cube.h"

// const float compressMul[] = {0 ,10.5, 94.6615, 355.184};
void Simulator::setPlayer(Character* player)
{
    if (_player != nullptr)
        delete _player;
    _player = player;
}

void Simulator::initialize(void)
{
    _player->initialize();
    _cube.cubeSizeChange(0.1);
    _cube.initialize();
    Animation* pushAnimation = findAnimation("idle");//find
    TimeNode node(getCurTimePoint(), getAfterTimePoint(pushAnimation->_animationMillisecond));
    _player->_baseAnimation.push_back({pushAnimation, node});
}

void Simulator::draw(void)
{
    _player->draw();
    _cube.draw();
}

void Simulator::update()
{
    std::chrono::steady_clock::time_point curTime = getCurTimePoint();
    _cube.update();
    _player->update(curTime, _cube._pos * _cube._vertex[0]);
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
        this->pushAnimation(pushAnimation, _player->_baseAnimation);
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
        this->pushAnimation(pushAnimation, _player->_baseAnimation);
    }
    else if (key == KeyInput::RUN)
    {
        if (_player->_baseAnimation.begin()->first->_name == "walk")
        {
            Animation* pushAnimation = findAnimation("run");//find
            this->pushAnimation(pushAnimation, _player->_baseAnimation);
        }
        else if (_player->_baseAnimation.begin()->first->_name == "run")
        {
            Animation* pushAnimation = findAnimation("walk");//find
            this->pushAnimation(pushAnimation, _player->_baseAnimation);
        }
    }
    else if (key == KeyInput::ATTACK && _player->_upperBodyAnimation.empty() == true)
    {
        Animation* pushAnimation = findAnimation("punch");//find
        this->pushAnimation(pushAnimation, _player->_upperBodyAnimation);
    }
    else if (key == KeyInput::JUMP && _player->_lowerBodyAnimation.empty() == true)
    {
        Animation* pushAnimation = findAnimation("runJump2");//find
        this->pushAnimation(pushAnimation, _player->_lowerBodyAnimation);
    }
    else if (key == KeyInput::CUBEBACK)
        _cube._pos = glm::translate(_cube._pos, glm::vec3(0,0,0.1));
    else if (key == KeyInput::CUBEFRONT)
        _cube._pos = glm::translate(_cube._pos, glm::vec3(0,0,-0.1));
    else if (key == KeyInput::CUBERIGHT)
        _cube._pos = glm::translate(_cube._pos, glm::vec3(0.1,0,0));
    else if (key == KeyInput::CUBELEFT)
        _cube._pos = glm::translate(_cube._pos, glm::vec3(-0.1,0,0));
    else if (key == KeyInput::CUBEUP)
        _cube._pos = glm::translate(_cube._pos, glm::vec3(0,0.1,0));
    else if (key == KeyInput::CUBEDOWN)
        _cube._pos = glm::translate(_cube._pos, glm::vec3(0,-0.1,0));
}

std::ostream& operator<<(std::ostream& os, const std::pair<Animation*, TimeNode>& ref)
{
    std::cout << "name : " << ref.first->_name << "\n";
    std::cout << "millisecond : " << ref.first->_animationMillisecond << std::endl;
    return os;
}
#include "include/Simulator.h"
#include "include/Animation.h"
#include "include/TimeNode.h"
#include "include/Character.h"
#include "include/Cube.h"
#include "include/EnumHeader.h"
// const float compressMul[] = {0 ,10.5, 94.6615, 355.184};
void Simulator::addPlayer(Character* player)
{
    _players.push_back(player);
}

void Simulator::initialize(void)
{
    addPlayer(_factory.makeCharacter(_skeleton, _controller));

    _cube.cubeSizeChange(0.1);
    _cube.initialize();
    _controller.setPlayer(_players.front());
    _controller.pushAnimation("idle", _animations, _controller.getPlayer()->_baseAnimation);

}

void Simulator::draw(void)
{
    for (Character* player : _players)
        player->draw();
    _cube.draw();
}

void Simulator::update()
{
    std::chrono::steady_clock::time_point curTime = getCurTimePoint();
    _cube.update();
    for (Character* player : _players)
        player->update(curTime, _cube._pos * _cube._vertex[0]);
}

void Simulator::changeAnimation(KeyInput key)
{
    if (key == KeyInput::CUBEBACK)
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
    else
        _controller.controllPlayer(key, _animations);
}

std::ostream& operator<<(std::ostream& os, const std::pair<Animation*, TimeNode>& ref)
{
    std::cout << "name : " << ref.first->_name << "\n";
    std::cout << "millisecond : " << ref.first->_animationMillisecond << std::endl;
    return os;
}
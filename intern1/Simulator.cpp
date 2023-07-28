#include "include/Simulator.h"
#include "include/Animation.h"
#include "include/TimeNode.h"
#include "include/Character.h"
#include "include/Cube.h"
#include "include/EnumHeader.h"
#include "include/GLM/gtx/quaternion.hpp"
// #include "include/GLM/gtc/quaternion.hpp"
// const float compressMul[] = {0 ,10.5, 94.6615, 355.184};
void Simulator::addPlayer(const std::string initAnimationName)//position, name 같은거 추가하면 될듯
{
    Character* newPlayer = _factory.makeCharacter(_skeleton, _controller);
    _controller.pushAnimation(initAnimationName, _animations, newPlayer->_baseAnimation);
    _players.push_back(newPlayer);
}

void Simulator::initialize(void)
{
    addPlayer("idle");
    _controller.initialize();
    _cube.cubeSizeChange(0.1);
    _cube.initialize();
    _controller.setPlayer(_players.front());
}

void Simulator::draw(void)
{
    for (Character* player : _players)
        player->draw();
    _cube.draw();
    _controller.draw();
}

void Simulator::update(void)
{
    std::chrono::steady_clock::time_point curTime = getCurTimePoint();
    _cube.update();
    _controller.update();
    for (Character* player : _players)
        player->update(curTime, _cube._pos * _cube._vertex[0]);
}
//현재 캐릭터가 보고있는 방향?
void Simulator::changeControllCharacter(void)
{
    const Character* curPlayer = _controller.getPlayer();
    float minRad = 5; 
    glm::mat4 inCharLocalPosition = _controller.getMatrixInCharLocal(BONEID::THORAX, curPlayer->getCharacterSkeleton(), curPlayer->getCharLocalVector());
    glm::mat4 tmpRot = (glm::mat3)inCharLocalPosition;
    glm::mat4 worRot = (glm::mat3)curPlayer->getCharacterWorldPosition();
    glm::vec3 curSee,curPos;
    curPos = curPlayer->getCharacterWorldPosition() * inCharLocalPosition * glm::vec4(0,0,0,1);
    curSee = worRot * tmpRot * glm::vec4(glm::cross(glm::vec3(1,0,0), curPlayer->getCharacterSkeleton().getBoneVector()[BONEID::THORAX]._direction),1);
    std::cout << glm::to_string(curSee) << std::endl;
    for (Character* player : _players)
    {
        if (player == curPlayer)
            continue;
        inCharLocalPosition = _controller.getMatrixInCharLocal(BONEID::THORAX, player->getCharacterSkeleton(), player->getCharLocalVector());
        glm::vec3 nextPos = player->getCharacterWorldPosition() * inCharLocalPosition * glm::vec4(0,0,0,1);
        glm::vec3 nextEeler = glm::normalize(nextPos - curPos);
        glm::quat quat = glm::rotation(curSee, nextEeler);
        float rad = std::abs(glm::angle(quat));
        // std::cout << glm::to_string(curSee) << std::endl;
        if (rad < minRad)
        {
            _controller.setPlayer(player);
            std::cout <<" rad : " << rad << std::endl;
            minRad = rad;
        }
    }
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
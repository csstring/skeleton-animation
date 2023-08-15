#include "include/Simulator.h"
#include "include/Animation.h"
#include "include/TimeNode.h"
#include "include/Character.h"
#include "include/Body/Cube.h"
#include "include/EnumHeader.h"
#include "include/GLM/gtx/quaternion.hpp"
#include "include/GLM/gtx/transform.hpp"
#include "include/Body/CollisionCylinder.h"
// #include "include/GLM/gtc/quaternion.hpp"
// const float compressMul[] = {0 ,10.5, 94.6615, 355.184};
void Simulator::addPlayer(const std::string initAnimationName)//position, name 같은거 추가하면 될듯
{
    float radius = _skeleton.getSkeletonWidth();
    float height = _skeleton.getSkeletonHeight();
    glm::vec3 rFoot = _skeleton.getCharLocalPosition(BONEID::RFOOT) - _skeleton.getCharLocalPosition(BONEID::ROOT);
    //fix me
    // CollisionCylinder* collisionMesh = _factory.makeCollisionCylinder(_physx.gScene, _physx.gPhysics, 1000, 1000, glm::vec3(0,rFoot.y,0));
    CollisionCylinder* collisionMesh = nullptr;
    Character* newPlayer = _factory.makeCharacter(_skeleton, _controller, collisionMesh);
    Character* oldPlayer = _controller.getPlayer();
    
    _players.push_back(newPlayer);
    _controller.setPlayer(newPlayer);
    _controller.pushAnimation(initAnimationName, _animations, BlendNode::BASE);
    _controller.setPlayer(oldPlayer);
}

void Simulator::initialize(void)
{
    _physx.Initialize();
    addPlayer("idle");
    _controller.initialize();

    glm::quat rot = glm::angleAxis(glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    _cube = new CollisionCube({0.001,0.001,0.001}, {0,-10.5,0}, rot);
    _cube->initialize(_physx.gPhysics, _physx.gScene);

    _ground.initialize();
    _controller.setPlayer(_players.front());
    _scene.initialize(_physx.gPhysics, _physx.gScene);
    _prevTime = getCurTimePoint();
    //tmp
    // physx::PxMaterial* material = _physx.gPhysics->createMaterial(0.5f, 0.5f, 0.5f);
    // physx::PxRigidStatic* plane = PxCreatePlane(*_physx.gPhysics, physx::PxPlane(0,1,0,0), *material);
    // _physx.gScene->addActor(*plane)
}

void Simulator::draw(void)
{
    for (Character* player : _players)
        player->draw();
    _cube->draw();
    _controller.draw();
    _scene.draw();
    // _ground.draw();
}

void Simulator::update(void)
{
    std::chrono::steady_clock::time_point curTime = getCurTimePoint();
    std::chrono::milliseconds  millisecond = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - _prevTime);
    float delta = static_cast<float>(millisecond.count()) / 1000.0f;
    if (delta == 0)
    {
        std::cout << "delta check\n";
        delta = 1.0f/60.0f;
    }
    float radian = PI * delta * 0.25f;
    glm::quat groundCubeRot = glm::angleAxis(radian, glm::vec3(0,1,0));
    _cube->update(groundCubeRot);
    _controller.update();
    _scene.update();
    // _ground.update();
    for (Character* player : _players)
    {
        player->update(curTime, _cube->_position , &_physx);
    }
    _physx.gScene->simulate(delta);
    //onContact
    _physx.gScene->fetchResults(true);

    _prevTime = curTime;
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
    for (Character* player : _players)
    {
        if (player == curPlayer)
            continue;
        inCharLocalPosition = _controller.getMatrixInCharLocal(BONEID::THORAX, player->getCharacterSkeleton(), player->getCharLocalVector());
        glm::vec3 nextPos = player->getCharacterWorldPosition() * inCharLocalPosition * glm::vec4(0,0,0,1);
        glm::vec3 nextEeler = glm::normalize(nextPos - curPos);
        glm::quat quat = glm::rotation(curSee, nextEeler);
        float rad = std::abs(glm::angle(quat));

        if (rad < minRad)
        {
            _controller.setPlayer(player);
            minRad = rad;
        }
    }
}

void Simulator::changeAnimation(KeyInput key)
{
    // if (key == KeyInput::CUBEBACK)
    //     _cube._pos = glm::translate(_cube._pos, glm::vec3(0,0,0.1));
    // else if (key == KeyInput::CUBEFRONT)
    //     _cube._pos = glm::translate(_cube._pos, glm::vec3(0,0,-0.1));
    // else if (key == KeyInput::CUBERIGHT)
    //     _cube._pos = glm::translate(_cube._pos, glm::vec3(0.1,0,0));
    // else if (key == KeyInput::CUBELEFT)
    //     _cube._pos = glm::translate(_cube._pos, glm::vec3(-0.1,0,0));
    if (key == KeyInput::CUBEUP)
        _ground._rot = glm::translate(_ground._rot, glm::vec3(0,0.1,0));
    else if (key == KeyInput::CUBEDOWN)
        _ground._rot = glm::translate(_ground._rot, glm::vec3(0,-0.1,0));
    else if (key == KeyInput::CUBEBACK)//j
        _ground._rot = _ground._rot * glm::rotate(glm::radians(1.0f), glm::vec3(-1,0,0));
    else if (key == KeyInput::CUBEFRONT)//u
        _ground._rot = _ground._rot * glm::rotate(glm::radians(1.0f), glm::vec3(1,0,0));
    else if (key == KeyInput::CUBERIGHT)//k
        _ground._rot = _ground._rot * glm::rotate(glm::radians(1.0f), glm::vec3(0,0,1));
    else if (key == KeyInput::CUBELEFT)//h
        _ground._rot = _ground._rot * glm::rotate(glm::radians(1.0f), glm::vec3(0,0,-1));
    else
        _controller.controllPlayer(key, _animations);
}

std::ostream& operator<<(std::ostream& os, const std::pair<Animation*, TimeNode>& ref)
{
    std::cout << "name : " << ref.first->_name << "\n";
    std::cout << "millisecond : " << ref.first->_animationMillisecond << std::endl;
    return os;
}
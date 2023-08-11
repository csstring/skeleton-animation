#pragma once

#include "Common.h"
#include "Skeleton.h"
#include <deque>
#include <iostream>
#include "Animation.h"
#include "Body/Cube.h"
#include "Controller.h"
#include "Body/BodyFactory.h"
#include "Body/Ground.h"
#include "Physx.h"
#include "Body/CollisionCube.h"
#include "include/Scene.h"
struct AnimationData;
class TimeNode;
class Character;
class Cube;
enum class KeyInput;
class Controller;
class CollisionCube;
class Simulator : Noncopyable
{
    private:
        std::vector<Character*> _players;//shared ptr써야할거 같은데
        Controller _controller;
        BodyFactory _factory;
        Physx _physx;
    public:
        Simulator(): _cube(glm::vec3(1.0f), glm::vec3(0.0f)){}
        ~Simulator(){}
        
        void initialize(void);
        void changeAnimation(KeyInput key);
        void update(void);
        void draw(void);
        void addPlayer(const std::string initAnimationName);//position, name 같은거 추가하면 될듯
        void changeControllCharacter(void);//캐릭터 id추가하고 구분해야 할듯
    public : 
        Skeleton               _skeleton;
        std::vector<Animation> _animations;
        CollisionCube          _cube;
        Ground                 _ground;
        Scene                  _scene;
};

// std::ostream& operator<<(std::ostream& os, const std::pair<Animation*, TimeNode>& ref);
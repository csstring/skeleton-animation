#pragma once
#include "common.h"
#include "Skeleton.h"
#include <deque>
#include <iostream>
#include "Animation.h"
#include "Cube.h"
#include "include/Controller.h"
#include "include/BodyFactory.h"

struct AnimationData;
class TimeNode;
class Character;
class Cube;
enum class KeyInput;
class Controller;

class Simulator : Noncopyable
{
    private:
        std::vector<Character*> _players;//shared ptr써야할거 같은데
        Controller _controller;
        BodyFactory _factory;
        
    private :
        Animation* findAnimation(const std::string& name);
        void pushAnimation(Animation* pushAnimation, std::deque<std::pair<Animation*, TimeNode>>& animationDeque);

    public:
        Simulator(){}
        ~Simulator(){}
        
        void initialize(void);
        void changeAnimation(KeyInput key);
        void update(void);
        void draw(void);
        void addPlayer(Character* player);

    public : 
        Skeleton               _skeleton;
        std::vector<Animation> _animations;
        Cube                   _cube;
};

// std::ostream& operator<<(std::ostream& os, const std::pair<Animation*, TimeNode>& ref);
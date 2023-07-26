#pragma once
#include "common.h"
#include "Skeleton.h"
#include <deque>
#include <iostream>
#include "Animation.h"
#include "Cube.h"
#include "Controller.h"
#include "BodyFactory.h"

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
        void addPlayer(const std::string initAnimationName);//position, name 같은거 추가하면 될듯
        void changeControllCharacter(void);//캐릭터 id추가하고 구분해야 할듯
    public : 
        Skeleton               _skeleton;
        std::vector<Animation> _animations;
        Cube                   _cube;
};

// std::ostream& operator<<(std::ostream& os, const std::pair<Animation*, TimeNode>& ref);
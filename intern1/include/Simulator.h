#pragma once
#include "common.h"
#include "Bone.h"
#include "Skeleton.h"
#include <deque>
#include <iostream>
#include "Animation.h"
#include "Cube.h"
struct AnimationData;
class TimeNode;
class Character;
class Cube;

enum class KeyInput
{
    UP,
    LOWERBACK,
    REFT,
    RIGHT,
    RUN,
    ATTACK,
    JUMP,
    STOP,
    CUBEFRONT,
    CUBEBACK,
    CUBERIGHT,
    CUBELEFT,
    CUBEUP,
    CUBEDOWN
};

class Simulator : Noncopyable
{
    private:
        Character* _player;

    private :
        Animation* findAnimation(const std::string& name);
        void pushAnimation(Animation* pushAnimation, std::deque<std::pair<Animation*, TimeNode>>& animationDeque);

    public:
        Simulator() : _player(nullptr){}
        ~Simulator()
        {
            if (_player != nullptr)
                delete _player;
        }
        void initialize(void);
        void changeAnimation(KeyInput key);
        void update(void);
        void draw(void);
        void setPlayer(Character* player);

    public : 
        Skeleton               _skeleton;
        std::vector<Animation> _animations;
        Cube                   _cube;
};

std::ostream& operator<<(std::ostream& os, const std::pair<Animation*, TimeNode>& ref);
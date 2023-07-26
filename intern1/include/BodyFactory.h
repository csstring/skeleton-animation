#pragma once

class Character;
class Skeleton;
class Controller;
class BodyFactory
{
    public:
        BodyFactory(){};
        ~BodyFactory(){};
        Character* makeCharacter(const Skeleton& _skeleton, const Controller& controller);
};

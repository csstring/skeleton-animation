#pragma once

class Character;
class Skeleton;
class BodyFactory
{
    public:
        BodyFactory(){};
        ~BodyFactory(){};
        Character* makeCharacter(const Skeleton& _skeleton);
};

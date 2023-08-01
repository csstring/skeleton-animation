#include "include/BodyFactory.h"
#include "include/Character.h"
#include "include/Skeleton.h"

Character* BodyFactory::makeCharacter(const Skeleton& _skeleton, const Controller& controller)
{
    Character* player = new Character(_skeleton, controller);
    player->initialize();
    return player;
}
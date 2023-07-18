#include "include/BodyFactory.h"
#include "include/Character.h"
#include "include/Skeleton.h"

Character* BodyFactory::makeCharacter(const Skeleton& _skeleton)
{
    return new Character(_skeleton);
}
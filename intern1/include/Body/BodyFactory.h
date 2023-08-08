#pragma once
#include "../Common.h"

class Character;
class Skeleton;
class Controller;
class CollisionCylinder;
class BodyFactory
{
    public:
        BodyFactory(){};
        ~BodyFactory(){};
        Character* makeCharacter(const Skeleton& _skeleton, const Controller& controller, CollisionCylinder* collisionMesh = nullptr);
        CollisionCylinder* makeCollisionCylinder(physx::PxScene* gScene, physx::PxPhysics* gPhysics, float radius, float height, glm::vec3 position);
};

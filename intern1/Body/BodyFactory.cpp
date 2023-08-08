#include "../include/Body/BodyFactory.h"
#include "../include/Character.h"
#include "../include/Skeleton.h"
#include "../include/Body/CollisionCylinder.h"
#include "../include/GLM/gtx/quaternion.hpp"
Character* BodyFactory::makeCharacter(const Skeleton& _skeleton, const Controller& controller, CollisionCylinder* collisionMesh)
{
    Character* player = new Character(_skeleton, controller, collisionMesh);
    player->initialize();
    return player;
}

///////body, collision, physx 인터페이스로 통합될거 같은데
CollisionCylinder* BodyFactory::makeCollisionCylinder(
    physx::PxScene* gScene, 
    physx::PxPhysics* gPhysics, 
    float radius, float height, 
    glm::vec3 position
)
{
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0,position.y,0)) * glm::toMat4(glm::rotation(glm::vec3(0,0,1), glm::vec3(0,1,0)));
    CollisionCylinder* _collision = new CollisionCylinder(radius,height,glm::vec3(0.0f), translate);
    _collision->initialize(gPhysics, gScene);
    return _collision;
}


#include "../include/Body/CollisionCube.h"
#include "../include/physx/PxPhysicsAPI.h"
#include "../include/GL/glew.h"
#include "../include/EnumHeader.h"
#include "../include/Body/Cube.h"
#include "../include/GLM/gtx/quaternion.hpp"
void CollisionCube::initialize(physx::PxPhysics* gPhysics, physx::PxScene* gScene)
{

    _cube.initialize();
    _cube._rot = _rot;

    physx::PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    physx::PxBoxGeometry box(_dimensions.x / 2, _dimensions.y / 2, _dimensions.z / 2);
    physx::PxShape* shape = gPhysics->createShape(box, *material);
    
    physx::PxFilterData filterData;
    {//지워도 될듯
        filterData.word0 = 1;
        shape->setSimulationFilterData(filterData);
    }

    physx::PxQuat rotation(_rot.x, _rot.y, _rot.z, _rot.w);
    gCubeActor = gPhysics->createRigidStatic(physx::PxTransform(_position.x, _position.y, _position.z, rotation));
    gCubeActor->attachShape(*shape);
    gScene->addActor(*gCubeActor);
}

void CollisionCube::update(glm::quat quat)
{
    auto pos = gCubeActor->getGlobalPose();
    pos.q.x = quat.x , pos.q.y = quat.y, pos.q.z = quat.z, pos.q.w = quat.w;
    gCubeActor->setGlobalPose(pos);

    _cube._rot = quat * _cube._rot;
    _cube.update();
}

void CollisionCube::draw(void)
{
    _cube.draw();
}
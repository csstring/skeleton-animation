#include "../include/Body/CollisionCube.h"
#include "../include/physx/PxPhysicsAPI.h"
#include "../include/GL/glew.h"
#include "../include/EnumHeader.h"
#include "../include/Body/Cube.h"
void CollisionCube::initialize(physx::PxPhysics* gPhysics, physx::PxScene* gScene)
{

    _cube.initialize();

    physx::PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    // Create box geometry
    physx::PxBoxGeometry box(_dimensions.x / 2, _dimensions.y / 2, _dimensions.z / 2);
    // Create shape
    physx::PxShape* shape = gPhysics->createShape(box, *material);
    // Create actor
    physx::PxFilterData filterData;
    filterData.word0 = 1; // You can choose the value based on your filtering requirements
    shape->setSimulationFilterData(filterData);
    
    gCubeActor = gPhysics->createRigidStatic(physx::PxTransform(_position.x, _position.y, _position.z));
    gCubeActor->attachShape(*shape);
    gScene->addActor(*gCubeActor);
}

void CollisionCube::update()
{
    _cube.update();
}

void CollisionCube::draw(void)
{
    _cube.draw();
}
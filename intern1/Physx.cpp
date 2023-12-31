#include "include/Physx.h"
#include "include/ContactCallBack.h"
using namespace physx;

PxFilterFlags MyCustomFilterShader(PxFilterObjectAttributes attributes0,
                                   PxFilterData filterData0,
                                   PxFilterObjectAttributes attributes1,
                                   PxFilterData filterData1,
                                   PxPairFlags& pairFlags,
                                   const void* constantBlock,
                                   PxU32 constantBlockSize)
{
    // Enable resolved collision and trigger the contact callback
    pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;

    return PxFilterFlag::eDEFAULT; // Or you can return other filter flags depending on your needs
}

void Physx::Initialize(void) 
{
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
    _contactCallback = new ContactCallBack();
    // Create a scene
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, 0.0f, 0.0f);
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.filterShader = MyCustomFilterShader;
    gScene = gPhysics->createScene(sceneDesc);
    gScene->setSimulationEventCallback(_contactCallback);

}

bool Physx::sweepTestUseSphere(float maxDistance, float radius, glm::vec3 initPose, glm::vec3 direction, physx::PxSweepBuffer &hit)
{
    physx::PxVec3 sweepDirection(direction.x,direction.y,direction.z);
    physx::PxTransform initialPose(physx::PxVec3(initPose.x, initPose.y, initPose.z));
    physx::PxSphereGeometry sphere(radius);
    return (gScene->sweep(sphere, initialPose, sweepDirection, maxDistance, hit));
}
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

void Physx::SimulateAndCheckCollisions() {
    // Step simulation
    gScene->simulate(1.0f / 60.0f);
    gScene->fetchResults(true);

    // Check for collisions
    PxContactPairHeader pairsHeader;
    PxContactPair pairs;
    // PxU32 nbPairs = gScene->getNpContactPairHeaderStreamCount();
    // PxContactStreamIterator iter(gScene->getNpContactPairHeaderStream());

    // while(iter.hasNextPair()) {
    //     iter.nextPair(pairsHeader);
    //     if((pairsHeader.actors[0] == gCylinderActor && pairsHeader.actors[1] == gBoxActor) ||
    //        (pairsHeader.actors[0] == gBoxActor && pairsHeader.actors[1] == gCylinderActor)) {
    //         // Collision between cylinder and box detected
    //         std::cout << "Collision detected between cylinder and box!" << std::endl;
    //         break;
    //     }
    // }
}

/*
PxFilterFlags MyCustomFilterShader(PxFilterObjectAttributes attributes0,
                                   PxFilterData filterData0,
                                   PxFilterObjectAttributes attributes1,
                                   PxFilterData filterData1,
                                   PxPairFlags& pairFlags,
                                   const void* constantBlock,
                                   PxU32 constantBlockSize)
{
    // Enable resolved collision and trigger the contact callback
    pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_CONTACT_POINTS;

    return PxFilterFlag::eDEFAULT; // Or you can return other filter flags depending on your needs
}
*/
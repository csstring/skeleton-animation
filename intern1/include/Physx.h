#pragma once
#include "Common.h"

class Physx
{
    public:
        physx::PxDefaultAllocator gAllocator;
        physx::PxDefaultErrorCallback gErrorCallback;
        physx::PxFoundation* gFoundation = nullptr;
        physx::PxPhysics* gPhysics = nullptr;
        physx::PxScene* gScene = nullptr;
        physx::PxSimulationEventCallback* _contactCallback = nullptr;

    public:
        Physx(){};
        ~Physx()
        {
            gScene->release();
            gPhysics->release();
            gFoundation->release();
        };
        void Initialize(void);
        void SimulateAndCheckCollisions(void);
};

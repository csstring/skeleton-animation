#pragma once
#include "Common.h"
#include "physx/PxPhysicsAPI.h"

class Physx
{
    public:
        physx::PxDefaultAllocator gAllocator;
        physx::PxDefaultErrorCallback gErrorCallback;
        physx::PxFoundation* gFoundation = nullptr;
        physx::PxPhysics* gPhysics = nullptr;
        physx::PxScene* gScene = nullptr;
        physx::PxRigidDynamic* gCylinderActor = nullptr; // Actor for the cylinder
        physx::PxRigidStatic* gBoxActor = nullptr; // Actor for the box
    
    public:
        Physx(){};
        ~Physx()
        {
            gScene->release();
            gPhysics->release();
            gFoundation->release();
        };
        void UpdateCylinderPosition(const glm::vec3& position);
        void Initialize(void);
        void CreateCylinder(float radius, float halfHeight, const glm::vec3& position);
        void CreateBox(const glm::vec3& dimensions, const glm::vec3& position);
        void SimulateAndCheckCollisions(void);
};

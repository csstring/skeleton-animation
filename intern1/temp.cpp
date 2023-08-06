#include <PxPhysicsAPI.h>
#include <glm/glm.hpp>

using namespace physx;

// PhysX objects
PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;
PxFoundation* gFoundation = nullptr;
PxPhysics* gPhysics = nullptr;
PxScene* gScene = nullptr;
PxRigidDynamic* gCylinderActor = nullptr; // Actor for the cylinder
PxRigidStatic* gBoxActor = nullptr; // Actor for the box

void InitializePhysX() {
    gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());

    // Create a scene
    PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    gScene = gPhysics->createScene(sceneDesc);
}

void CreateCylinder(float radius, float halfHeight, const glm::vec3& position) {
    // Create material
    PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    // Create capsule geometry
    PxCapsuleGeometry capsule(radius, halfHeight);

    // Create shape
    PxShape* shape = gPhysics->createShape(capsule, *material);

    // Create actor
    gCylinderActor = gPhysics->createRigidDynamic(PxTransform(position.x, position.y, position.z));
    gCylinderActor->attachShape(*shape);
    gScene->addActor(*gCylinderActor);
}

void CreateBox(const glm::vec3& dimensions, const glm::vec3& position) {
    // Create material
    PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

    // Create box geometry
    PxBoxGeometry box(dimensions.x / 2, dimensions.y / 2, dimensions.z / 2);

    // Create shape
    PxShape* shape = gPhysics->createShape(box, *material);

    // Create actor
    gBoxActor = gPhysics->createRigidStatic(PxTransform(position.x, position.y, position.z));
    gBoxActor->attachShape(*shape);
    gScene->addActor(*gBoxActor);
}

void UpdateCylinderPosition(const glm::vec3& position) {
    gCylinderActor->setGlobalPose(PxTransform(position.x, position.y, position.z));
}

void SimulateAndCheckCollisions() {
    // Step simulation
    gScene->simulate(1.0f / 60.0f);
    gScene->fetchResults(true);

    // Check for collisions
    PxContactPairHeader pairsHeader;
    PxContactPair pairs;
    PxU32 nbPairs = gScene->getNpContactPairHeaderStreamCount();
    PxContactStreamIterator iter(gScene->getNpContactPairHeaderStream());

    while(iter.hasNextPair()) {
        iter.nextPair(pairsHeader);
        if((pairsHeader.actors[0] == gCylinderActor && pairsHeader.actors[1] == gBoxActor) ||
           (pairsHeader.actors[0] == gBoxActor && pairsHeader.actors[1] == gCylinderActor)) {
            // Collision between cylinder and box detected
            std::cout << "Collision detected between cylinder and box!" << std::endl;
            break;
        }
    }
}

int main(int, char**) {
    InitializePhysX();

    CreateCylinder(1.0f, 2.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    CreateBox(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(10.0f, 10.0f, 10.0f));

    // Simulate and check collisions
    SimulateAndCheckCollisions();

    // Release resources
    gScene->release();
    gPhysics->release();
    gFoundation->release();

    return 0;
}

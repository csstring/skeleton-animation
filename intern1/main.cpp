#include "include/GLM/glm.hpp"
#include "include/GLFW/glfw3.h"
#pragma comment(lib, "OpenGL32.lib")
#include "include/Window.h"
#include "include/shader.h"
#include "include/Skeleton.h"
#include "include/Animation.h"
#include "include/CmuFileParser.h"
#include "include/Simulator.h"
#include "include/AMCFileParser.h"
#include "include/Body/Ground.h"
#include "include/AnimationCompressor.h"
#include "include/Body/BodyFactory.h"
#include "include/Camera.h"
#include "include/Body/Cube.h"
#include <chrono>
#include <cmath>
#include <iostream>
// physx::PxDefaultAllocator gAllocator;
// physx::PxDefaultErrorCallback gErrorCallback;
// physx::PxFoundation* gFoundation = nullptr;
// physx::PxPhysics* gPhysics = nullptr;
// physx::PxScene* gScene = nullptr;
// physx::PxSimulationEventCallback* _contactCallback = nullptr;
// using namespace physx;

// class ContactReportCallback : public PxSimulationEventCallback
// {
// public:
//     void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override
//     {
//         // Iterate through contact pairs
//         for (PxU32 i = 0; i < nbPairs; i++)
//         {
//             const PxContactPair& cp = pairs[i];
//             if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
//             {
//                 std::cout << "Collision detected between objects!" << std::endl;
//             }
//         }
//     }
//      // Other callback methods
//     void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) override {}
//     void onWake(PxActor** actors, PxU32 count) override {}
//     void onSleep(PxActor** actors, PxU32 count) override {}
//     void onTrigger(PxTriggerPair* pairs, PxU32 count) override {}
//     void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) override {}
//     // Other callback methods as before...
// };

// PxFilterFlags contactReportFilterShader(
//     PxFilterObjectAttributes attributes0, PxFilterData filterData0,
//     PxFilterObjectAttributes attributes1, PxFilterData filterData1,
//     PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
// {
//     pairFlags = PxPairFlag::eCONTACT_DEFAULT
//               | PxPairFlag::eNOTIFY_TOUCH_FOUND;
//     return PxFilterFlag::eDEFAULT;
// }

// int main()
// {
//     // Foundation
//     PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

//     // Physics
//     PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true);

//     // Scene description
//     PxSceneDesc sceneDesc(physics->getTolerancesScale());
//     sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
//     sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
//     ContactReportCallback contactReportCallback;
//     sceneDesc.simulationEventCallback = &contactReportCallback;
//     sceneDesc.filterShader = contactReportFilterShader;

//     // Create scene
//     PxScene* scene = physics->createScene(sceneDesc);

//     // Material
//     PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.5f);

//     // Create static plane
//     PxRigidStatic* plane = PxCreatePlane(*physics, PxPlane(0,1,0,0), *material);

//     scene->addActor(*plane);

//     // Create dynamic box
//     PxTransform boxPosition = PxTransform(PxVec3(0,5,0));
//     PxRigidDynamic* box = PxCreateDynamic(*physics, boxPosition, PxBoxGeometry(1,1,1), *material, 10.0f);
//     scene->addActor(*box);

//     // Simulation loop
//     for(int i = 0; i < 2000; ++i)
//     {
//         scene->simulate(1.0f/60.0f);
//         scene->fetchResults(true);
//         auto copy = box->getGlobalPose().p;
//         glm::vec3 copy0 = {copy.x, copy.y, copy.z};
//         // std::cout << "box mesh pos : "<< glm::to_string(copy0) << std::endl;
//     }

//     // Cleanup
//     box->release();
//     plane->release();
//     material->release();
//     scene->release();
//     physics->release();
//     foundation->release();

//     return 0;
// }
std::chrono::system_clock::time_point start;
std::chrono::duration<double>sec;
const float compressMul[] = {0 ,10.5, 94.6615, 355.184};
Camera      _camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (_camera._isFirst)
    {
        _camera._lastX = xpos;
        _camera._lastY = ypos;
        _camera._isFirst = false;
    }

    float xoffset = xpos - _camera._lastX;
    float yoffset = _camera._lastY - ypos;

    _camera._lastX = xpos;
    _camera._lastY = ypos;

    _camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    _camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void fileLoad(Simulator& simulator)
{
    std::vector<const char*> amcPathList = {"./amc/walk1.amc", 
    "./amc/run.amc","./amc/runJump2.amc","./amc/punch.amc",
    "./amc/idle.amc", "./amc/dance.amc", "./amc/drink.amc", 
    "./amc/roll.amc", "./amc/golf.amc"};
    simulator._animations.push_back(Animation("walk", 1));
    simulator._animations.push_back(Animation("run", 1));
    simulator._animations.push_back(Animation("runJump2", 1));
    simulator._animations.push_back(Animation("punch", 1));
    simulator._animations.push_back(Animation("idle", 1));
    simulator._animations.push_back(Animation("dance", 1));
    simulator._animations.push_back(Animation("drink", 1));
    simulator._animations.push_back(Animation("roll", 1));
    simulator._animations.push_back(Animation("golf", 1));
    CmuFileParser parser(asfPath,&simulator._skeleton, &simulator._animations[0]);
    parser.loadCmuFile();

    AnimationCompressor compressor;
    AnimationData root = simulator._animations[0]._rootNode;
    for (int i = 0; i < simulator._animations.size(); ++i)
    {
        simulator._animations[i]._rootNode = root;
        AMCFileParser amcParser(amcPathList[i], &simulator._skeleton, &simulator._animations[i]);
        amcParser.loadAMCFile();
        // if (simulator._animations[i]._name == "dance")
        //     compressor.CompressData(&simulator._animations[i], 355.184/100000000.0f);
    }
}

int main() 
{
    Window window;
    Shader shader("/shaderSource/VertexShader.glsl","/shaderSource/FragmentShader.glsl");
    
    window.initialize();
    _camera.initialize();
    shader.initialize();

    Simulator simulator;
    BodyFactory bodyFactory;
    fileLoad(simulator);
    simulator.initialize();
    //비슷한 지점 찾기
    //simulator._animations[4].findSameFrame(10);
    while (window.isWindowClose() == false)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        window.processInput(simulator, _camera);
        glm::mat4 projection = glm::perspective(glm::radians(_camera._zoom), (float)WINDOW_WITH / (float)WINDOW_HEIGHT, 0.1f, 500.0f);
        _camera.update();
        shader.setMat4("projection", projection);
        shader.setMat4("view", _camera._view);
        simulator.update();
        simulator.draw();
        window.bufferSwap();
        glfwPollEvents();
    }
}
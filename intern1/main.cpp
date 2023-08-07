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
#include "include/Ground.h"
#include "include/AnimationCompressor.h"
#include "include/BodyFactory.h"
#include "include/Camera.h"
#include "include/Cube.h"
#include <chrono>
#include <cmath>

std::chrono::system_clock::time_point start;
std::chrono::duration<double>sec;
const float compressMul[] = {0 ,10.5, 94.6615, 355.184};
Camera      _camera;
physx::PxDefaultAllocator gAllocator;
physx::PxDefaultErrorCallback gErrorCallback;
physx::PxFoundation* gFoundation = nullptr;
physx::PxPhysics* gPhysics = nullptr;
physx::PxScene* gScene = nullptr;
physx::PxRigidDynamic* gCylinderActor = nullptr; // Actor for the cylinder
physx::PxRigidStatic* gBoxActor = nullptr; // Actor for the box


using namespace physx;

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

    //camera mouse call
    // glfwSetFramebufferSizeCallback(window._window, framebuffer_size_callback);
    // glfwSetCursorPosCallback(window._window, mouse_callback);
    // glfwSetScrollCallback(window._window, scroll_callback);
    // glfwSetInputMode(window._window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    InitializePhysX();

    CreateCylinder(1.0f, 2.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    CreateBox(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(10.0f, 10.0f, 10.0f));

    // Simulate and check collisions
    SimulateAndCheckCollisions();

    // Release resources
    gScene->release();
    gPhysics->release();
    gFoundation->release();

}
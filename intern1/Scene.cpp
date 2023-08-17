#include "include/Scene.h"
#include <random>
#include "include/GLM/gtc/random.hpp"
void Scene::draw(void)
{
    for(CollisionCube& cube: _objects)
        cube.draw();
}

void Scene::update(void)
{
    for(CollisionCube& cube: _objects)
    {
        auto pos = cube.gCubeActor->getGlobalPose();
        // std::cout << ft_to_string(pos.p) << std::endl;
        // std::cout << "angle " << pos.q.getAngle() <<std::endl;
        cube.update();
    }
}

void Scene::initialize(physx::PxPhysics* gPhysics, physx::PxScene* gScene)
{
    glm::vec3 pos1(0.0f,-10.0f, 0.0f);
    glm::vec3 pos2(0.0f,-10.0f, 0.0f);
    glm::vec3 dimension1(5.0f, 0.2f,12.0f);
    glm::vec3 dimension2(10.0f, 0.2f,12.0f);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i=0; i <30; ++i)
    {
        pos1.x += 5;
        pos1.y += 1;
        if (i % 2)
            pos1.z = 0;
        else
            pos1.z = 0;
        glm::vec3 randomColor = glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f));
        _objects.push_back(CollisionCube(dimension1, pos1));
    }
    for (int i=0; i <10; ++i)
    {
        pos2.x -= 10;
        pos2.y += 2;
        if (i % 2)
            pos2.z = 0;
        else
            pos2.z = 0;
        glm::vec3 randomColor = glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f));
        _objects.push_back(CollisionCube(dimension2, pos2));
    }
    glm::quat rot = glm::angleAxis(glm::radians(12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    _objects.push_back(CollisionCube({60,0.1,30}, {-30,0,-20}, rot));
    rot = glm::angleAxis(glm::radians(-12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    _objects.push_back(CollisionCube({60,0.1,30}, {30,0,-20}, rot));

    // rot = glm::angleAxis(glm::radians(-8.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // _objects.push_back(CollisionCube({50,1,50}, {0,-10.5,0}, rot, glm::vec3(1,0,0)));
    for(CollisionCube& cube: _objects)
        cube.initialize(gPhysics, gScene);
}
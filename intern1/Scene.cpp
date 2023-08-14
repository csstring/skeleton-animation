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
        cube.update();
}

void Scene::initialize(physx::PxPhysics* gPhysics, physx::PxScene* gScene)
{
    glm::vec3 pos(0.0f,-10.0f, 0.0f);
    glm::vec3 dimension(10.0f, 0.2f,12.0f);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    for (int i=0; i <30; ++i)
    {
        pos.x += 10;
        pos.y += 1;
        if (i % 2)
            pos.z = 0;
        else
            pos.z = 0;
        glm::vec3 randomColor = glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f));
        _objects.push_back(CollisionCube(dimension, pos));
    }
    // glm::quat rot = glm::angleAxis(glm::radians(8.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // _objects.push_back(CollisionCube({50,1,50}, {0,-10.5,0}, rot));

    // rot = glm::angleAxis(glm::radians(-8.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // _objects.push_back(CollisionCube({50,1,50}, {0,-10.5,0}, rot, glm::vec3(1,0,0)));
    for(CollisionCube& cube: _objects)
        cube.initialize(gPhysics, gScene);
}
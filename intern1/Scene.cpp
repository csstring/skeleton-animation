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
    glm::vec3 pos(0.0f);
    glm::vec3 dimension(4.0f,1.0f,1.0f);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Generate random values for each component of the vec3
    // for (int i=0; i <10; ++i)
    // {
    //     glm::vec3 next;
    //     glm::vec3 randomColor = glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f));
    //     _objects.push_back(CollisionCube(dimension, pos, randomColor));
    //     next = dimension * 1.4f;
    //     pos.z -= dimension.z * 0.5f + next.z * 0.5f;
    //     dimension = next;  
    // }
    _objects.push_back(CollisionCube({50,2,50}, {0,-11,0}, {0,1,0}));
    for(CollisionCube& cube: _objects)
        cube.initialize(gPhysics, gScene);
}
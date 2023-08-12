#pragma once
#include "Cube.h"
#include "../Common.h"

class CollisionCube
{
    public:
        Cube  _cube;
        glm::vec3 _position;
        glm::vec3 _dimensions;
        glm::quat _rot;
        physx::PxRigidStatic* gCubeActor = nullptr;
        
    public:
        CollisionCube(glm::vec3 dimensions, glm::vec3 position, glm::quat rot = glm::quat(1.0f, glm::vec3(0.0f)), glm::vec3 color = glm::vec3(0,1,0)) : _cube(dimensions,position, color)
        {
            _position = position;
            _dimensions = dimensions;
            _rot = rot;
        };
        ~CollisionCube(){};
        void initialize(physx::PxPhysics* gPhysics, physx::PxScene* gScene);
        void update(void);
        void draw(void);
};

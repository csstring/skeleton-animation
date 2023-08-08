#pragma once
#include "Cube.h"
#include "../Common.h"

class CollisionCube
{
    public:
        Cube  _cube;
        glm::vec3 _position;
        glm::vec3 _dimensions;
        physx::PxRigidStatic* gCubeActor = nullptr;
        
    public:
        CollisionCube(glm::vec3 dimensions, glm::vec3 position) : _cube(dimensions,position)
        {
            _position = position;
            _dimensions = dimensions;
        };
        ~CollisionCube(){};
        void initialize(physx::PxPhysics* gPhysics, physx::PxScene* gScene);
        void update(void);
        void draw(void);
};

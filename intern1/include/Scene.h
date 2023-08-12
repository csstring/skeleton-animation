#pragma once
#include "Common.h"
#include "Body/CollisionCube.h"

class Scene : public Noncopyable
{
    private:
        std::vector<CollisionCube> _objects;
    public:
        Scene(){};
        ~Scene(){};
        void draw();
        void update(void);
        void initialize(physx::PxPhysics* gPhysics, physx::PxScene* gScene);
};
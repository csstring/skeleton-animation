#pragma once
#include "Cylinder.h"
#include "../Common.h"

class CollisionCylinder
{
public:
    uint32 _VAO, _VBO, _VCO;
    float _radius, _height;
    Cylinder  _cylinder;
    glm::vec3 _position;
    physx::PxRigidDynamic* gCylinderActor = nullptr;
public:
    CollisionCylinder(float radius, float height, glm::vec3 position, glm::mat4 cylinderRot) : _cylinder(radius,height,12, cylinderRot)
    {
        _position = position;
        _radius = radius;
        _height = height;
    };
    ~CollisionCylinder(){};
    void UpdateCylinderPosition(const glm::vec3& position);
    void initialize(physx::PxPhysics* gPhysics, physx::PxScene* gScene);
    void update(glm::mat4 translate);
    void draw(void);
};

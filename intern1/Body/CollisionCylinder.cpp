#include "../include/Body/CollisionCylinder.h"
#include "../include/physx/PxPhysicsAPI.h"
#include "../include/GL/glew.h"
#include "../include/EnumHeader.h"
void CollisionCylinder::initialize(physx::PxPhysics* gPhysics, physx::PxScene* gScene)
{
    ///실린더 init으로 빼야 할 부분
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &_VCO);
    glBindBuffer(GL_ARRAY_BUFFER, _VCO);
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    _cylinder.initialize(glm::vec3(0,1,0), _VCO, BONEID::THORAX);

    physx::PxMaterial* material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
    // Create capsule geometry
    physx::PxCapsuleGeometry capsule(_radius, _height/2);
    // Create shape
    physx::PxShape* shape = gPhysics->createShape(capsule, *material);
    // Create actor
    physx::PxFilterData filterData;
    filterData.word0 = 1; // You can choose the value based on your filtering requirements
    shape->setSimulationFilterData(filterData);

    gCylinderActor = gPhysics->createRigidDynamic(physx::PxTransform(_position.x, _position.y, _position.z));
    gCylinderActor->attachShape(*shape);
    gScene->addActor(*gCylinderActor);
}
//collision update fix me
// void Physx::UpdateCylinderPosition(const glm::vec3& position) 
// {
//     gCylinderActor->setGlobalPose(PxTransform(position.x, position.y, position.z));
// }
void CollisionCylinder::update(glm::mat4 translate)
{
    glm::vec3 pos = translate * glm::vec4(0,0,0,1);
    gCylinderActor->setGlobalPose(physx::PxTransform(pos.x, pos.y, pos.z));
    auto copy = gCylinderActor->getGlobalPose().p;
    glm::vec3 copy1 = {copy.x, copy.y, copy.z};
    // std::cout << "collision mesh pos : "<< glm::to_string(copy1) << std::endl;
    for (uint32 i = 0; i < _cylinder._buffer.size(); ++i)
    {
        _cylinder._buffer[i] = translate * _cylinder._vertices[i];
    }
}
//glDrawArrays(GL_LINES, 0, _cylinder._numVerticesSide);
void CollisionCylinder::draw(void)
{
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * _cylinder._buffer.size(), _cylinder._buffer.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, _cylinder._numVerticesSide);
    glDrawArrays(GL_LINE_LOOP, _cylinder._numVerticesSide+1, _cylinder._numVerticesTopBottom-1);
    glDrawArrays(GL_LINE_LOOP, _cylinder._numVerticesSide + _cylinder._numVerticesTopBottom+1, _cylinder._numVerticesTopBottom-1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
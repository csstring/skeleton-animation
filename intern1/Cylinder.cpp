#include "include/Cylinder.h"
#include "include/GL/glew.h"
#include <cmath>

void Cylinder::initialize()
{
    const float angleStep = 2.0f * PI / static_cast<float>(_numSlices);
    float curAngle = 0.0f;
    std::vector<glm::vec3> cylinderPos;

    cylinderPos.reserve(_numSlices);
    for (uint32 i = 0; i < _numSlices; ++i)
    {
        glm::vec3 pos;

        pos.x = glm::cos(curAngle) * _radius;
        pos.y = glm::sin(curAngle) * _radius;
        cylinderPos.push_back(pos);
        curAngle += angleStep;
    }

    _vertices.reserve(_numVerticesTotal);
    for (const glm::vec3& pos : cylinderPos)
    {
        glm::vec4 vertexTop(pos.x, pos.y , -_heigth ,1);
        glm::vec4 vertexBottom(pos.x, pos.y, 0, 1);

        _vertices.push_back(_transForm * vertexTop);
        _vertices.push_back(_transForm * vertexBottom);
    }

    glm::vec4 topCenter(0, 0, -_heigth, 1);
    _vertices.push_back(_transForm * topCenter);
    for (const glm::vec3& pos :cylinderPos)
    {
        glm::vec4 vertexTop(pos.x, pos.y , -_heigth ,1);
        _vertices.push_back(_transForm * vertexTop);
    }

    glm::vec4 bottomCenter(0, 0, 0, 1);
    _vertices.push_back(_transForm * bottomCenter);
    for (const glm::vec3& pos :cylinderPos)
    {
        glm::vec4 vertexBottom(pos.x, pos.y, 0, 1);
        _vertices.push_back(_transForm * vertexBottom);
    }
}

void Cylinder::render(uint32 VBO) const
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * _vertices.size(), _vertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, _numVerticesSide);
    // Render to cover
    glDrawArrays(GL_TRIANGLE_FAN, _numVerticesSide, _numVerticesTopBottom);
    // Render bottom cover
    glDrawArrays(GL_TRIANGLE_FAN, _numVerticesSide + _numVerticesTopBottom, _numVerticesTopBottom);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
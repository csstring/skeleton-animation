#include "../include/Body/Cube.h"
#include "../include/GL/glew.h"
#include "../include/GLM/gtx/quaternion.hpp"
void Cube::initialize(void)
{
    _vertex = CreateCubeVertices(_dimenstion, _position);
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * _vertex.size(), _vertex.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::vector<glm::vec3> colors;
    colors.resize(_vertex.size(), _color);
    glGenBuffers(1, &_VCO);
    glBindBuffer(GL_ARRAY_BUFFER, _VCO);
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
    _buffer = _vertex;
}

void Cube::update(void)
{
    for (uint32 i = 0; i < _buffer.size(); ++i)
    {
        _buffer[i] = _translate * glm::toMat4(_rot) * _vertex[i];
    }
}

void Cube::draw(void)
{
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * _buffer.size(), _buffer.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

std::vector<glm::vec4> Cube::CreateCubeVertices(const glm::vec3& dimensions, const glm::vec3& position) {
    float halfWidth = dimensions.x / 2.0f;
    float halfHeight = dimensions.y / 2.0f;
    float halfDepth = dimensions.z / 2.0f;

    std::vector<glm::vec4> vertices;
    vertices.reserve(36);

    glm::vec4 cubeVertices[8] = {
        {position.x - halfWidth, position.y - halfHeight, position.z - halfDepth, 1.0f},
        {position.x - halfWidth, position.y - halfHeight, position.z + halfDepth, 1.0f},
        {position.x - halfWidth, position.y + halfHeight, position.z + halfDepth, 1.0f},
        {position.x - halfWidth, position.y + halfHeight, position.z - halfDepth, 1.0f},
        {position.x + halfWidth, position.y - halfHeight, position.z - halfDepth, 1.0f},
        {position.x + halfWidth, position.y - halfHeight, position.z + halfDepth, 1.0f},
        {position.x + halfWidth, position.y + halfHeight, position.z + halfDepth, 1.0f},
        {position.x + halfWidth, position.y + halfHeight, position.z - halfDepth, 1.0f}
    };

    int indices[] = {
        1, 5, 6,  6, 2, 1,  // Front face
        4, 0, 3,  3, 7, 4,  // Back face
        0, 1, 2,  2, 3, 0,  // Left face
        5, 4, 7,  7, 6, 5,  // Right face
        2, 6, 7,  7, 3, 2,  // Top face
        5, 1, 0,  0, 4, 5   // Bottom face
    };

    for (int i = 0; i < 36; i++) {
        vertices.push_back(cubeVertices[indices[i]]);
    }

    return vertices;
}
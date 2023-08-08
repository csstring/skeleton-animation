#include "../include/Body/Cube.h"
#include "../include/GL/glew.h"

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
    colors.resize(_vertex.size(), glm::vec3(0,1,0));
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
        _buffer[i] = _pos * _vertex[i];
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

void Cube::cubeSizeChange(float size)
{
    for (uint32 i = 0; i < _vertex.size(); ++i)
    {
        _vertex[i].x *= size;
        _vertex[i].y *= size;
        _vertex[i].z *= size;
    }
}

std::vector<glm::vec4> Cube::CreateCubeVertices(const glm::vec3& dimensions, const glm::vec3& position) {
    float halfWidth = dimensions.x / 2.0f;
    float halfHeight = dimensions.y / 2.0f;
    float halfDepth = dimensions.z / 2.0f;

    std::vector<glm::vec4> vertices;
    vertices.reserve(36);

    int indices[] = {0, 1, 2, 3, 0, 4, 5, 6, 5, 1, 7, 6, 4, 3, 5, 4, 5, 7, 2, 1, 6, 3, 2, 7};

    glm::vec3 cubeVertices[8];
    cubeVertices[0] = {position.x - halfWidth, position.y - halfHeight, position.z - halfDepth};
    cubeVertices[1] = {position.x - halfWidth, position.y - halfHeight, position.z + halfDepth};
    cubeVertices[2] = {position.x - halfWidth, position.y + halfHeight, position.z + halfDepth};
    cubeVertices[3] = {position.x - halfWidth, position.y + halfHeight, position.z - halfDepth};
    cubeVertices[4] = {position.x + halfWidth, position.y - halfHeight, position.z - halfDepth};
    cubeVertices[5] = {position.x + halfWidth, position.y - halfHeight, position.z + halfDepth};
    cubeVertices[6] = {position.x + halfWidth, position.y + halfHeight, position.z + halfDepth};
    cubeVertices[7] = {position.x + halfWidth, position.y + halfHeight, position.z - halfDepth};

    for (int i = 0; i < 24; i += 3) {
        vertices.push_back(glm::vec4(cubeVertices[indices[i]], 1.0f));
        vertices.push_back(glm::vec4(cubeVertices[indices[i + 1]], 1.0f));
        vertices.push_back(glm::vec4(cubeVertices[indices[i + 2]], 1.0f));
    }

    return vertices;
}
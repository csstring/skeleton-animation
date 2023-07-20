#include "include/Cube.h"
#include "include/GL/glew.h"

void Cube::initialize(void)
{
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
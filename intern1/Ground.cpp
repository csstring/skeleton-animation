#include "include/Ground.h"
#include "include/GL/glew.h"

void Ground::initialize(void)
{
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, _groundVertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::vector<glm::vec3> colors = {glm::vec3(1,0,0),glm::vec3(0,1,0),glm::vec3(0,0,1),glm::vec3(1,0,0)};
    glGenBuffers(1, &_VCO);
    glBindBuffer(GL_ARRAY_BUFFER, _VCO);
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);//size 열의 개수
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindVertexArray(0);
}

void Ground::draw(void) //깊이버퍼 이상한데?
{
    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
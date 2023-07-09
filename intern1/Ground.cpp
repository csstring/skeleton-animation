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
    glBindVertexArray(0);

}

void Ground::draw(void)
{
    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
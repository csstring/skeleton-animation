#include "include/Bone.h"
#include "include/GL/glew.h"

#include <iostream>

void Bone::draw(void) const 
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0,1);
    glBindVertexArray(0);
}

void Bone::setupBone(void)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_postion), &_postion, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);
}
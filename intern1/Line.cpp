#include "include/Line.h"
#include "include/GL/glew.h"

void Line::initialize(glm::vec3 color, uint32 VBC)
{
    glm::vec4 bottomPoint(0,0,0,1);
    glm::vec4 topPoint(0,0,-_length,1);
    std::vector<glm::vec3> colors;

    _vertices.push_back(_transForm * bottomPoint);
    _vertices.push_back(_transForm * topPoint);
    colors.push_back(color);
    colors.push_back(color);

    glBindBuffer(GL_ARRAY_BUFFER, VBC);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), colors.data(), GL_STATIC_DRAW);
}

void Line::render(uint32 VBO) const
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * _vertices.size(), _vertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, 2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
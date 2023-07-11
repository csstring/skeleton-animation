#pragma once
#include "include/Common.h"
#include "include/GLM/glm.hpp"
#include <vector>

class Line
{
    public:
        float     _length;
        glm::mat4 _transForm;
        std::vector<glm::vec4> _vertices;

    public:
        Line(float length, glm::mat4 transForm) :_length(length), _transForm(transForm){};
        ~Line(){};
        void initialize(glm::vec3 color, uint32 VBC);
        void render(uint32 VBO) const;
    private:
};

#pragma once
#include "include/Common.h"
#include "include/GLM/glm.hpp"
#include <vector>

class Cylinder
{
    public:
        float     _radius;
        float     _heigth;
        uint32    _numSlices;
        glm::mat4 _transForm;
        uint32 _numVerticesSide;
        uint32 _numVerticesTopBottom;
        uint32 _numVerticesTotal;
        std::vector<glm::vec4> _vertices;
    public:
        Cylinder(
            float radius, float height, uint32 numSilices, glm::mat4 transForm
        ) :_radius(radius), _heigth(height), _numSlices(numSilices), _transForm(transForm)
        {
            _numVerticesSide = (_numSlices + 1) * 2;
            _numVerticesTopBottom = _numSlices + 2;
            _numVerticesTotal = _numVerticesSide + _numVerticesTopBottom * 2;
        }
        ~Cylinder(){}
        void initialize(glm::vec3 color, uint32 VBC, BONEID ID);
        void render(uint32 vbo) const;
    private:
};

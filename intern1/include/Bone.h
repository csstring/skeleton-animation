#pragma once
#include "common.h"
#include "GLM/glm.hpp"
#include <string>
#include <vector>

enum class DOF{
    RX,
    RY,
    RZ,
    TX,
    TY,
    TZ
};

struct Bone
{
    uint32 _boneIndex;
    float _length;
    std::string _boneName;
    glm::vec3 _postion;
    glm::vec3 _axis;
    glm::vec3 _color;
    glm::vec3 _direction;
    std::vector<DOF> _dof;
};


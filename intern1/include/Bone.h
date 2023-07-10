#pragma once
#include "common.h"
#include <string>
#include <vector>
#include "include/GLM/ext.hpp"

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
    uint32           _boneIndex;
    std::string      _boneName;
    float            _length;
    glm::vec3        _postion;
    glm::vec3        _axis;
    glm::vec3        _direction;
    std::vector<DOF> _dof;//
    glm::quat        _c;//
    glm::quat        _invC;//
    glm::vec3        _b; //amc에서 local정하기
};


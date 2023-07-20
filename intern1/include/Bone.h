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
    uint32           _parentBoneIndex;
    std::string      _boneName;
    float            _length;
    glm::vec3        _direction;
    
    //fixme : 아래쪽 데이터 필요없어서 정리 해야함
    glm::vec3        _postion;
    glm::vec3        _axis;
    std::vector<DOF> _dof;
    glm::quat        _c;
    glm::quat        _invC;
    glm::vec3        _b;
};

